{
    if( $1 in already ) {
        next
    }

    already[$1] = "x"
    print $0
}
