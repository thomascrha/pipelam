pkgname=pipelam
pkgver=0.1.7
pkgrel=1
pkgdesc="A lightweight GTK4-based notification system for displaying text, images, and progress bars"
arch=('x86_64' 'aarch64')
url="https://github.com/thomascrha/pipelam"
license=('MIT')
depends=('gtk4' 'gtk4-layer-shell')
makedepends=('clang' 'make' 'scdoc')
optdepends=('systemd: for systemd socket activation')
source=("$pkgname-$pkgver.tar.gz::https://github.com/thomascrha/pipelam/archive/refs/tags/v$pkgver.tar.gz")
sha256sums=('5c30c55d59f3e632cf0c9093f200671caaff90680e00f12e57a5838b4a53c242')

build() {
    cd "$pkgname-$pkgver"
    make build
    make docs
}

package() {
    cd "$pkgname-$pkgver"
    make PREFIX="$pkgdir/usr" install

    # Install license
    install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"

    # Install systemd service and socket files
    install -Dm644 systemd/pipelam.service "$pkgdir/usr/lib/systemd/user/pipelam.service"
    install -Dm644 systemd/pipelam.socket "$pkgdir/usr/lib/systemd/user/pipelam.socket"

    # # Install config file
    # install -d "$pkgdir/etc/pipelam"
    # install -Dm644 config/pipelam.toml "$pkgdir/etc/pipelam/pipelam.toml"
}

