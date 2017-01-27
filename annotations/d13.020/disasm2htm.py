# Crappy script to turn the output of a Radare2 disassembly listing
# into a HTML document (which still looks like plain text),
# but with links from caller (e.g. after 'bl') to callee ("(fcn)").
#
# 'My first Python script', by DL4YHF, 2017-01-21 .
#
# If you also use Python once per year (and 'C' 364 days a year) :
# Doing everything 'the most possible pythonic way' is just crazy .
# Don't miss pythoncentral.io/cutting-and-slicing-strings-in-python/ .
#


import sys

def sym2link(symbol):
    return '<a href="#'+symbol+'">'+symbol+'</a>'

def sym2noLink(symbol):
    return '<font color="red"><u>'+symbol+'</u></font>'

def sym2anchor(symbol):
    return '<a id="'+symbol+'"><b>'+symbol+'</b></a>'

class DisasmToHTML():
    in_lines = [];
    functions= {};  # all functions CALLED FROM or SHOWN IN the listing.
    funcs_listed = set(); # all functions really SHOWN in the listing.
    current_function = "";

    def __init__(self):
        pass

    def writeHTMLPart1(self):
        self.of.write("<!doctype html>\n<html>\n<head>\n<style>\n" )

        # Styles for a multi-column index (function overview or similar) :
        self.of.write("ul#MultiColumn{\n")
        self.of.write(" -moz-column-count: 3;\n")  # Will web browsers..
        self.of.write(" -moz-column-gap: 10px;\n") # ..ever speak a common language ?
        self.of.write(" -webkit-column-count: 3;\n")
        self.of.write(" -webkit-column-gap: 10px;\n")
        self.of.write(" column-count: 3;\n")
        self.of.write(" column-gap: 10px;\n")
        self.of.write(" }\n")
        self.of.write("ul#MultiColumn li{\n")
        self.of.write("  line-height: 1.5em;\n")
        self.of.write("  display: block;\n")
        self.of.write(" }\n")
        self.of.write("ul#MultiColumn .double li {\n")
        self.of.write("   width: 50%;\n")
        self.of.write(" }\n")
        self.of.write("</style>\n</head>\n<body>\n")

    def addFunctionAndCaller(self, sym_name, current_function):
        if len(sym_name) > 0:
            if sym_name not in self.functions:
                # there's no known caller for this function yet
                # (otherwise it would already be a key in 'functions').
                # So FIRST append 'sym_name' to 'functions' ...
                self.functions[sym_name] = set() # not '{}' - that's a
            if current_function != '':
                # Function 'sym_name' is being called from another
                #   function (current_function) so add it to the *set* of callers.
                # (the same function shall not appear twice in the "list" of callers,
                #  thus use a Python-set here, not a Python-list.
                self.functions[sym_name].add(current_function)

    def processFile(self, filename_without_extension):
        self.current_function = "";

        # Read the raw disassembler listing into memory
        # (the result would be an array of strings in C, guess here it's a list)
        with open(filename_without_extension+'.txt') as infile:
            self.in_lines = infile.read().splitlines(False);
            # no need to close infile here .. "with" will close it

        # Pass 1 : Find all function names, and store in a dictionary
        for line in self.in_lines:
            # look for special sequences in the disassembly created by Radare2:
            if line.startswith("/ (fcn) ") : # in an annotated function now
                # e.g. Radare2 output: "/ (fcn) Reset_Handler 8"
                sym_name = line.rsplit(' ',2)[1]
                # rsplit explained at docs.python.org/2/library/stdtypes.html
                self.addFunctionAndCaller(sym_name, "")
                self.funcs_listed.add(sym_name) # mark as LISTED function,
                   # so it will be shown in the index, and displayed as a
                   # 'clickable' link (not a red 'broken' link) later.
                current_function = sym_name # here: for annotated FUNCTION
            elif line[12:14]=="0x" : # Looks like Radare2's disassembly
                # Not sure if Radare2 always aligns mnemonics to column 44
                # so look for subroutine calls beginning in column 40:
                token  = " bl "; # token is a 'str' (not a 'string' !!)
                column = line.find(token,40);
                if column >= 40 : # should be followed by callee's name or address
                    # Because many callees are not annotated yet, treat their
                    # hexadecimal address like a name, so we can link to them .
                    # e.g. 'RCC_Init' called from 'SystemInit', but also
                    #      '0x804e102' called from '0x0804e2f4'.
                    # Beware: Radare2 omits leading zeroes in OPERANDS,
                    #         but not in the ADDRESS column !
                    sym_name = line[column:].split()[1]
                    # About split: docs.python.org/2.7/library/strings.html .
                    #   split[0] would be the mnemonic (branch with link)
                    #   split[1] should be the operand (=callee) .
                    # sym_name must be a function (because it's CALLED here),
                    # which isn't necessarily disassembled yet !
                    # Despite that, list it in the dictionary 'functions',
                    # to avoid TWO passes (one to find all functions,
                    # and another to find all CALLERS of those functions).
                    # Expected to get here (1st time) with:
                    # sym_name="RCC_Init",  current_function="SystemInit".
                    self.addFunctionAndCaller( sym_name, current_function)
                # end if < saw instruction ' bl ' in a disassembly line >
            # end if < line with a hex address in column 12 (?) >
            elif line[12:16] == ";-- ":  # another annotation in Radare2 ?
                # This is not the same as an annotated FUNCTION (R2: "af+")
                # but it may be the target for a branch, call, or IRQ handler,
                #     e.g.: "|           ;-- TIM7_DAC_IRQHandler:"
                # To keep it simple, treat the symbol like a function:
                sym_name = line[16:-1] # exclude the trailing colon !
                self.addFunctionAndCaller(sym_name, current_function)
                self.funcs_listed.add(sym_name) # mark as LISTED function, too
                current_function = sym_name  # here: for 'f'lagged label (?)
                # (useful for interrupt vectors that aren't "af+"ed)

            if line.startswith("\  ") : # not in an annotated FUNCTION anymore
                current_function = ""   # prevent wrong 'caller' entries

        # <-- end of the loop to extract info from the raw assembly listing

        # Create the output file: HTML-ish, but 'almost' plain text only.
        #   No fancy templating, keep simple things simple.
        self.of = open(filename_without_extension+'.htm', 'w')
        self.writeHTMLPart1()

        # Emit a 'linked' list of disassembled functions (each function name will be an HTML anchor)
        self.of.write("<h1>Disassembled Function Overview</h1>\n")
        self.of.write("<a id='Functions'></a><ul id='MultiColumn'>\n")
        nFuncsCalledButNotListed = 0
        for sym_name in sorted(self.functions):
            if sym_name in self.funcs_listed: # not just CALLED but SHOWN in the listing:
                self.of.write('<li> <a href="#'+sym_name+'">'+sym_name+'</a><br>\n')
            else:
                nFuncsCalledButNotListed = nFuncsCalledButNotListed+1
        self.of.write("</ul>\n")

        if nFuncsCalledButNotListed > 0:
            self.of.write("<h2>Functions called further below but not shown in the disassembly listing yet</h1>\n")
            self.of.write(" (modify the r2 file, add a line with 'pdf @FuncName', to have them listed further below)<br>\n")
            self.of.write("<a id='Functions'></a><ul id='MultiColumn'>\n")
            for sym_name in sorted(self.functions):
                if sym_name not in self.funcs_listed: # CALLED but not SHOWN in the listing:
                    self.of.write('<li> <font color="red"><u>'+sym_name+'</u></font>\n')
            self.of.write("</ul>\n")

        # Pass 2 : Append the listing itself, with internal links and anchors:
        self.of.write("<pre><code>")
        for line in self.in_lines:
            out_line = line
            r_parts = line.rsplit(' ',1)
            if line.startswith("/ (fcn) "):  # e.g. "/ (fcn) Reset_Handler 8"
                sym_name = line.rsplit(' ',2)[1]  # anchor for the function name (last but one)
                out_line = line.replace(sym_name, sym2anchor(sym_name) )
                if sym_name in self.functions : # known function...
                    callers = self.functions[sym_name]
                    if callers: # Are there any KNOWN callers for this function ?
                        # emit the 'fcn' line, and list this function's callers (if any)
                        self.of.write(out_line + "\n") # emit original line with 'fcn'
                        out_line = '|       Caller:'
                        # 'callers' is a SET which will be converted into a
                        #  space-separated list of hyperlinks here :
                        for caller in callers:
                           # If the line gets too long, flush it and begin
                           # a new. May look ugly in a wide-screen browser.
                           # The raw Radare2 output had over 130 columns..
                           if len(out_line) > 220:
                               self.of.write(out_line + "\n")
                               out_line = '|              '
                           if caller in self.funcs_listed:
                               out_line = out_line + ' ' + sym2link( caller )
                           else:
                               out_line = out_line + ' ' + sym2noLink( caller )
            # end if < begin of a dissassembled function >
            elif r_parts[-1] in self.functions:
                sym_name = r_parts[-1]
                # The LAST word in the line may be the name of a function.
                # Radare2 adds the name of any annotated symbol in the last
                #         column, depending on the operand, e.g.:
                # > ldr r0, [0x080f924c] ; [0x80f924c:4]=0x8094359 SystemInit
                # If the last word in a line is contained in f_names,
                # turn it into a 'clickable' or 'non-functional' link:
                if sym_name in self.funcs_listed:
                    out_line = r_parts[0] + ' ' + sym2link(sym_name)
                else: # no idea what the symbol is.. show a "bad link":
                    out_line = r_parts[0] + ' ' + sym2noLink(sym_name)

            elif line[12:14]=="0x" : # Looks like Radare2's disassembly,
                # with lots of space before the hexadecimal code address.
                # Not sure if Radare2 always aligns mnemonics to column 42..44
                # so look for subroutine calls beginning in column 40:
                token  = " bl "; # 'branch with link' (subroutine call)
                column = line.find(token,40);
                if column >= 40 : # token at the right place for a mnemonic..
                    l_split = line[column:].split(token)
                    # e.g. l_split = ['', 'RCC_Init   ; blah, blah' ]
                    l_split = l_split[1].split() # skip optional comments, etc
                    sym_name = l_split[0] # separate the operand (callee after 'bl')
                    if sym_name in self.functions:
                        # if Callee is a disassembled function link to it,
                        # else show a broken link similar as above:
                        if sym_name in self.funcs_listed:
                            out_line = line.replace(sym_name, sym2link(sym_name) )
                        else:
                            out_line = line.replace(sym_name, sym2noLink(sym_name) )
                    # ( if the callee is just a hex address, we can't add a
                    #   link to it because the disassembly listing only
                    #   contains 'annotated' (named) functions.
                    #   All those appear like a 'red broken link' in Wikipedia,
                    #   in THIS case the broken link can be fixed by extending
                    #   the Radare2 script, e.g. file disasm_yhf.r or similar )
            # end if < line could be R2 disassembly >
            elif line[12:16] == ";-- ": # another annotation in Radare2 ?
                sym_name = line[16:-1]  # exclude the trailing colon !
                if sym_name in self.functions:
                    out_line = line.replace(sym_name, sym2anchor(sym_name))
            # end if < line could have been annotated by 'f', not 'af+' >

            self.of.write(out_line+"\n") # emit 'original' or 'modified' line

        # End of the disassembly listing, finish the HTML file:
        self.of.write("</code></pre>\n</body>\n</html>")
        self.of.close()

    # end disasm2htm()

# end class DisasmToHTML

if __name__ == "__main__":
    n_args = len(sys.argv)-1  # obviously C-like, the "program name" counts as an argument, too
    d2h = DisasmToHTML()
    if n_args==1 :    # ONE real argument: must be the name of the raw disassmbly w/o extension
       d2h.processFile(sys.argv[1])
    elif n_args == 0: # NO real argument specified : use the default, produced by disasm_yhf.r 
       d2h.processFile( 'listing' )
    else:
       print("Usage: disasm2htm <filename_without_extension> ")