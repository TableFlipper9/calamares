#!/usr/bin/env python3
import os
import shutil
import urllib.request
import tarfile
import subprocess
import libcalamares
import glob
import re
import sys
import time
import hashlib

def _progress_hook(count, block_size, total_size):
    _check_parent_alive()
    percent = int(count * block_size * 100 / total_size)
    if percent > 100:
        percent = 100
    libcalamares.job.setprogress(percent / 2)

def _check_parent_alive():
    if os.getppid() == 1:
        sys.exit(1)

def _check_global_storage_keys():
    """Check if required global storage keys are set and have values."""
    print("Checking global storage keys...")
    
    if not libcalamares.globalstorage.contains("FINAL_DOWNLOAD_URL"):
        raise Exception("FINAL_DOWNLOAD_URL key is not set in global storage")
    
    if not libcalamares.globalstorage.contains("STAGE_NAME_TAR"):
        raise Exception("STAGE_NAME_TAR key is not set in global storage")
    
    final_download_url = libcalamares.globalstorage.value("FINAL_DOWNLOAD_URL")
    stage_name_tar = libcalamares.globalstorage.value("STAGE_NAME_TAR")
    
    if final_download_url.endswith('/'):
        final_download_url = final_download_url.rstrip('/')
    
    if not final_download_url:
        raise Exception("FINAL_DOWNLOAD_URL key exists but has no value")
    
    if not stage_name_tar:
        raise Exception("STAGE_NAME_TAR key exists but has no value")
    
    print(f"FINAL_DOWNLOAD_URL variable: {final_download_url}")
    print(f"STAGE_NAME_TAR variable: {stage_name_tar}")
    
    return final_download_url, stage_name_tar

def _safe_run(cmd):
    _check_parent_alive()
    try:
        proc = subprocess.Popen(cmd)
        while True:
            retcode = proc.poll()
            if retcode is not None:
                if retcode != 0:
                    sys.exit(1) 
                return retcode
            if os.getppid() == 1:
                proc.terminate()
                try:
                    proc.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    proc.kill()
                sys.exit(1)
            time.sleep(1)
    except subprocess.SubprocessError:
        sys.exit(1)

def verify_pgp_signature_gemato(filepath, data_file=None):
    _check_parent_alive()
    gentoo_keys = "/usr/share/openpgp-keys/gentoo-release.asc"
    expected_key_id = "13EBBDBEDE7A12775DFDB1BABB572E0E2D182910"
    
    try:
        if data_file:
            cmd = ["gemato", "openpgp-verify-detached", "-K", gentoo_keys, filepath, data_file]
        else:
            cmd = ["gemato", "openpgp-verify", "-K", gentoo_keys, filepath]
        
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            check=False
        )
        
        if result.returncode == 0:
            output = result.stdout + result.stderr
            if expected_key_id in output or expected_key_id.lower() in output.lower():
                if result.stdout:
                    print(result.stdout.strip())
                if result.stderr:
                    print(result.stderr.strip())
                return (True, "Verified with gemato")
            else:
                libcalamares.utils.error(f"Signature not from expected key {expected_key_id}")
                return (False, "Wrong signing key")
        else:
            libcalamares.utils.error(f"PGP signature verification failed for {filepath}")
            if result.stderr:
                libcalamares.utils.error(result.stderr.strip())
            return (False, "Verification failed")
    except FileNotFoundError:
        libcalamares.utils.error("gemato command not found")
        return (False, "gemato not found")
    except Exception as e:
        libcalamares.utils.error(f"Error verifying PGP signature: {str(e)}")
        return (False, str(e))

def calculate_hash(filepath, hash_type):
    _check_parent_alive()
    try:
        if hash_type == "SHA512":
            hasher = hashlib.sha512()
        elif hash_type == "BLAKE2B":
            hasher = hashlib.blake2b(digest_size=64)
        else:
            print(f"FAILURE: Unsupported hash type: {hash_type}")
            return None
        with open(filepath, 'rb') as f:
            while chunk := f.read(8192):
                _check_parent_alive()
                hasher.update(chunk)
        return hasher.hexdigest()
    except Exception as e:
        print(f"FAILURE: Error calculating {hash_type} hash: {str(e)}")
        return None

def verify_hash(filepath, hash_type, expected_hash):
    calculated_hash = calculate_hash(filepath, hash_type)
    if calculated_hash is None:
        return False
    if calculated_hash.lower() == expected_hash.lower():
        return True
    else:
        print(f"FAILURE: {hash_type} verification for {os.path.basename(filepath)}")
        print(f"Expected: {expected_hash}")
        print(f"Got:      {calculated_hash}")
        return False

def parse_digests_file(digests_path):
    _check_parent_alive()
    file_hashes = {}
    current_hash_type = None
    try:
        with open(digests_path, 'r') as f:
            for line in f:
                _check_parent_alive()
                line = line.strip()
                if line.startswith('-') or line.startswith('Hash:'):
                    continue
                hash_header_match = re.match(r'^#\s+([A-Z0-9]+)\s+HASH$', line)
                if hash_header_match:
                    current_hash_type = hash_header_match.group(1)
                    continue
                if current_hash_type:
                    hash_line_match = re.match(r'^([0-9a-f]+)\s+(.+)$', line)
                    if hash_line_match:
                        hash_value = hash_line_match.group(1)
                        filename = hash_line_match.group(2)
                        if filename not in file_hashes:
                            file_hashes[filename] = {}
                        file_hashes[filename][current_hash_type] = hash_value
        return file_hashes
    except Exception as e:
        print(f"FAILURE: Error parsing DIGESTS file: {str(e)}")
        return {}

def verify_stage3_with_digests(digests_path, stage3_path):
    success, msg = verify_pgp_signature_gemato(digests_path)
    if not success:
        libcalamares.utils.error("DIGESTS signature verification failed")
        return False
    else:
        print(f"DIGESTS PGP verification: {msg}")
    
    file_hashes = parse_digests_file(digests_path)
    if not file_hashes:
        print("FAILURE: No files found in DIGESTS")
        return False
    
    stage3_name = os.path.basename(stage3_path)
    if stage3_name not in file_hashes:
        print(f"FAILURE: {stage3_name} not found in DIGESTS file")
        return False
    
    hashes = file_hashes[stage3_name]
    all_valid = True
    for hash_type, expected_hash in hashes.items():
        if not verify_hash(stage3_path, hash_type, expected_hash):
            all_valid = False
    return all_valid

def run():
    if (libcalamares.globalstorage.contains("GENTOO_LIVECD") and 
        libcalamares.globalstorage.value("GENTOO_LIVECD") == "yes"):
        print("GENTOO_LIVECD is set to 'yes', copying /run/rootfsbase to rootMountPoint")
        
        root_mount_point = libcalamares.globalstorage.value("rootMountPoint")
        if not root_mount_point:
            raise Exception("rootMountPoint not set in global storage")
        
        _safe_run(["rsync", "-aXA", "--hard-links", "--info=progress2", "/run/rootfsbase/", root_mount_point + "/"])
        
        return None

    final_download_url, stage_name_tar = _check_global_storage_keys()
    
    full_tarball_url = final_download_url
    full_sha256_url = final_download_url + ".sha256"
    full_tarball_asc_url = final_download_url + ".asc"
    base_url = '/'.join(final_download_url.split('/')[:-1])
    full_digests_url = base_url + "/" + stage_name_tar + ".DIGESTS"

    download_path = f"/mnt/{stage_name_tar}"
    sha256_path = f"/mnt/{stage_name_tar}.sha256"
    tarball_asc_path = f"/mnt/{stage_name_tar}.asc"
    digests_path = f"/mnt/{stage_name_tar}.DIGESTS"
    
    root_mount_point = libcalamares.globalstorage.value("rootMountPoint")
    if not root_mount_point:
        raise Exception("rootMountPoint not set in global storage")
    extract_path = root_mount_point

    if os.path.exists(download_path):
        os.remove(download_path)
    if os.path.exists(sha256_path):
        os.remove(sha256_path)
    if os.path.exists(tarball_asc_path):
        os.remove(tarball_asc_path)
    if os.path.exists(digests_path):
        os.remove(digests_path)

    urllib.request.urlretrieve(full_tarball_url, download_path, _progress_hook)
    libcalamares.job.setprogress(30)
    
    urllib.request.urlretrieve(full_sha256_url, sha256_path)
    libcalamares.job.setprogress(35)
    
    try:
        urllib.request.urlretrieve(full_tarball_asc_url, tarball_asc_path)
    except Exception as e:
        print(f"WARNING: Could not download tarball .asc file: {str(e)}")
    libcalamares.job.setprogress(38)
    
    try:
        urllib.request.urlretrieve(full_digests_url, digests_path)
    except Exception as e:
        print(f"WARNING: Could not download DIGESTS file: {str(e)}")
    libcalamares.job.setprogress(40)

    _safe_run(["bash", "-c", f"cd /mnt && sha256sum -c {stage_name_tar}.sha256"])
    libcalamares.job.setprogress(43)

    if os.path.isfile(tarball_asc_path):
        success, msg = verify_pgp_signature_gemato(tarball_asc_path, download_path)
        if not success:
            libcalamares.utils.error("Tarball PGP signature verification failed")
            sys.exit(1)
        else:
            print(f"Tarball PGP verification: {msg}")
    else:
        print("WARNING: No PGP signature file for tarball")
    libcalamares.job.setprogress(46)

    if os.path.isfile(digests_path) and not verify_stage3_with_digests(digests_path, download_path):
        libcalamares.utils.error("DIGESTS verification failed")
        sys.exit(1)
    libcalamares.job.setprogress(50)

    with tarfile.open(download_path, "r:xz") as tar:
        members = tar.getmembers()
        total_members = len(members)
        for i, member in enumerate(members):
            _check_parent_alive()
            try:
                tar.extract(member, extract_path)
            except OSError as e:
                if e.errno != 17:
                    raise
            libcalamares.job.setprogress(50 + (i * 50 / total_members))

    os.remove(download_path)
    if os.path.exists(sha256_path):
        os.remove(sha256_path)
    if os.path.exists(tarball_asc_path):
        os.remove(tarball_asc_path)
    if os.path.exists(digests_path):
        os.remove(digests_path)

    shutil.copy2("/etc/resolv.conf", os.path.join(extract_path, "etc", "resolv.conf"))
    os.makedirs(os.path.join(extract_path, "etc/portage/binrepos.conf"), exist_ok=True)
    
    gentoobinhost_source = "/etc/portage/binrepos.conf/gentoobinhost.conf"
    if os.path.exists(gentoobinhost_source):
        shutil.copy2(
            gentoobinhost_source,
            os.path.join(extract_path, "etc/portage/binrepos.conf/gentoobinhost.conf")
        )
    else:
        print(f"Warning: {gentoobinhost_source} does not exist, skipping copy")

    _safe_run(["chroot", extract_path, "getuto"])

    package_use_dir = os.path.join(extract_path, "etc/portage/package.use")
    os.makedirs(package_use_dir, exist_ok=True)
    with open(os.path.join(package_use_dir, "00-livecd.package.use"), "w", encoding="utf-8") as f:
        f.write(">=sys-kernel/installkernel-50 dracut\n")

    _safe_run([
        "chroot", extract_path, "/bin/bash", "-c",
        'emerge-webrsync -q'
    ])

    is_systemd = "systemd" in stage_name_tar.lower()
    
    partitions = libcalamares.globalstorage.value("partitions")
    is_encrypted = False
    if partitions:
        for partition in partitions:
            if partition.get("mountPoint") == "/" and "luksMapperName" in partition:
                is_encrypted = True
                break
    
    packages = "sys-boot/grub net-misc/networkmanager net-wireless/iwd sys-fs/cryptsetup"

    _safe_run([
        "chroot", extract_path, "/bin/bash", "-c",
        f'EMERGE_DEFAULT_OPTS="${{EMERGE_DEFAULT_OPTS}} --getbinpkg" emerge -q {packages}'
    ])

    _safe_run([
        "chroot", extract_path, "/bin/bash", "-c",
        'EMERGE_DEFAULT_OPTS="${EMERGE_DEFAULT_OPTS} --getbinpkg" emerge -q1 app-text/asciidoc sys-apps/dbus sys-libs/timezone-data'
    ])

    for folder in ["distfiles", "binpkgs"]:
        path = os.path.join(extract_path, f"var/cache/{folder}")
        if os.path.exists(path):
            for entry in glob.glob(path + "/*"):
                if os.path.isfile(entry) or os.path.islink(entry):
                    os.unlink(entry)
                elif os.path.isdir(entry):
                    shutil.rmtree(entry)

    gentoo_repo = os.path.join(extract_path, "var/db/repos/gentoo")
    if os.path.exists(gentoo_repo):
        shutil.rmtree(gentoo_repo)
        print("Removed /var/db/repos/gentoo to free RAM")

    write_dracut_config(extract_path, stage_name_tar)

    return None

def write_dracut_config(root_mount_point, stage_name_tar):
    """Write dracut configuration before kernel installation to prevent warnings.
    
    This runs BEFORE gentoopkg installs gentoo-kernel-bin, so when installkernel's
    dracut hook runs, it will find this config and generate correct initramfs first time.
    """
    dracut_conf_dir = os.path.join(root_mount_point, "etc/dracut.conf.d")
    os.makedirs(dracut_conf_dir, exist_ok=True)
    
    dracut_conf_path = os.path.join(dracut_conf_dir, "10-calamares.conf")
    
    partitions = libcalamares.globalstorage.value("partitions")
    is_encrypted = False
    if partitions:
        for partition in partitions:
            if partition.get("mountPoint") == "/" and "luksMapperName" in partition:
                is_encrypted = True
                break
    
    is_systemd = "systemd" in stage_name_tar.lower()
    
    with open(dracut_conf_path, 'w') as conf_file:
        conf_file.write("# Generated by Calamares installer\n")
        conf_file.write("# Configuration for dracut initramfs generation\n\n")
        
        conf_file.write('hostonly="yes"\n')
        conf_file.write('hostonly_cmdline="yes"\n\n')
        
        if not is_systemd:
            conf_file.write("# Exclude systemd modules\n")
            conf_file.write('omit_dracutmodules+=" systemd systemd-initrd systemd-networkd dracut-systemd plymouth "\n\n')
            
            if is_encrypted:
                conf_file.write("# Add encryption support (OpenRC)\n")
                conf_file.write('add_dracutmodules+=" crypt dm rootfs-block "\n')
        else:
            conf_file.write("# Omit unnecessary modules (systemd system)\n")
            conf_file.write('omit_dracutmodules+=" plymouth "\n')
    
    print(f"Pre-configured dracut at {dracut_conf_path} (systemd={is_systemd}, encrypted={is_encrypted})")

