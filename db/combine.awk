{
    if( $1 in already ) {
        next
    }

    already[$1] = 0
    print $0
}
