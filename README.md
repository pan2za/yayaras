# YAYARAS (Yet Another YARA Scanner)

YAYARAS (Yet Another YARA Scanner) enables malware researchers to triage samples using OSINT quickly.

Often when scanning files during triage we need to act fast, OSINT can help us along so often we find ourselves downloading and scanning files with OSINT YARA signatures. This process is typically done by scripting the current version of YARA's scanner. With YAYARAS you can scan files using a ZIP file containing YARA signatures. It will ignore the errors in those third-party repositories and provide you the matches on the signatures that compiled correctly. The next steps will be to implement features like files within encrypted ZIP files so you don't even need to open up the files before having an idea of what is going on. More features like pulling down ZIPs from the internet and configuration files for the OSINT signature URLs you want is also planned to be added.

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
