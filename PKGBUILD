# Author: mosra <mosra@centrum.cz>
pkgname=map2x-qt
pkgver=dev
pkgrel=1
pkgdesc="Qt GUI for Map2X navigation system"
arch=('i686' 'x86_64')
url="http://mosra.cz/blog/map2x.php"
license=('LGPLv3')
makedepends=('cmake')
depends=('map2x-core')
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
