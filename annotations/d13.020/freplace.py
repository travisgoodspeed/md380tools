import sys

def freplace(filename, old_string, new_string):
    # We support stuff like "\r\n" which the shell doesn't un-escape,
    # so un-escape the search pattern and replacement (strings):
    unescaped_old = old_string.decode('string_escape')
    unescaped_new = new_string.decode('string_escape')

    # Safely read the input file using 'with' .
    # Since we don't want Python to mis-interpret the END-OF-LINE markers,
    # open it in binary mode (hoping that 'rb' isn't just another myth..)
    with open(filename, mode='rb') as f:
        s = f.read()
        if unescaped_old not in s:
            print '"{old_string}" not found in {filename}.'.format(**locals())
            return

    # Safely write the changed content, if found in the file
    with open(filename, 'wb') as f:
        print 'Changing "{old_string}" to "{new_string}" in {filename}'.format(**locals())
        s = s.replace(unescaped_old, unescaped_new)
        f.write(s)


if __name__ == "__main__":
    if len(sys.argv) == 4:  # obviously C-like, the "program name" counts as an argument, too
       freplace(sys.argv[1], sys.argv[2], sys.argv[3])
    else:
       print("Usage: freplace <filename> <old_string> <new_string> ")