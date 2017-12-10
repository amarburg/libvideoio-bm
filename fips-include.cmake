
# Need to give importers the abilit to find to CGo library
set( GOLIB_PATH "$ENV{GOPATH}/src/github.com/amarburg/cgo-lazyquicktime" )
fips_include_directories( ${GOLIB_PATH} )
