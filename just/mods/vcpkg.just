set windows-shell := ["powershell.exe", "-NoLogo", "-NoProfile", "-Command"]
set working-directory := "../.."

# Advance the vcpkg registry baseline to its latest commit.
bump:
    vcpkg x-update-baseline
