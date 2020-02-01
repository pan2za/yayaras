# YAYARAS

YAYARAS (Yet Another YARA Scanner) enables malware researchers to triage samples using OSINT quickly.

# Current Features
- Scan single file using ZIP files containing YARA signatures
- Scan single file using single YARA signature file

# Dependencies
- libmagic
- libzip
- libcurl
- libyara

# Installation
```bash
git clone https://github.com/lillypad/yayaras.git
cd yayaras/
make
make install
```

# Uninstall
```
make uninstall
```

# Using YAYARAS
```bash
yayaras -i repo.zip -s sample.bin
yayaras -i sig.yara -s sample.bin
```

# Features to Come
- Scan files using URLs pointing to YARA signatures or YARA ZIP repositories
- Scan files using configuration file containing list of files / URLs
- Scan folders with multi-threading
- Scan ZIP encrypted files
- Backup validated YARA signatures to ZIP
- Scan files within encrypted ZIP archives
- Other report formats like JSON
- Validate folders or ZIP files of YARA signatures for errors and namespace collisions for use in CI
