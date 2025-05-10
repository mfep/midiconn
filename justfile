build-flatpak:
    flatpak-builder --install --user --force-clean ../midiconn-build ./data/packaging/xyz.safeworlds.midiconn.yml
