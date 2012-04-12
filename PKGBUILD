# Author: mosra <mosra@centrum.cz>
pkgname=kompas-qt
pkgver=dev
pkgrel=1
pkgdesc="Qt GUI for Kompas navigation software"
arch=('i686' 'x86_64')
url="http://mosra.cz/blog/kompas.php"
license=('LGPLv3')
makedepends=('cmake')
depends=('kompas-core' 'qt')
options=(!strip)

build() {
    mkdir -p "$startdir/build"
    cd "$startdir/build"

    cmake .. \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DBUILD_TESTS=TRUE
    make
}

package() {
  cd "$startdir/build"
  make DESTDIR="$pkgdir/" install
}
