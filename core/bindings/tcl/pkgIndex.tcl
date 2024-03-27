# pkgIndex.tcl description for loading a package

puts "This is run\n"

package ifneeded "SubstanceConnector" 0.1 \
    [list load [file join $dir connector[info sharedlibextension]]]
