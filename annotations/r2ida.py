import sys
"""
A minimalistic script to turn r2 firmware annotations to ida python commands.
CCa is interpreted as comments, 
fa+ as defining and renaming functions
and flags are treated as comments and added to bookmarks 

Copy the output of the script to File-> Script Command,
chose python as script language, paste and execute.

Bookmarks are accessible via crtl+m

Can easily be broken by unexpected non-alphanum chars...

"""
idc_commands = []
marks = 0
def main():
	global marks
	if len(sys.argv) != 2:
		print "./r2ida.py <annotations_file>"
		sys.exit(0)
	with open(sys.argv[1],"r") as f:
		annotations = f.readlines()
		for annotation in annotations:
			if annotation.startswith("CCa"):
				cca = annotation.split(" ")
				address = cca[1]
				comment = " ".join(cca[2:]).strip().replace('"','\\"')
				idc_command = 'idc.MakeComm(%s,"%s")'%(address,comment)
				idc_commands.append(idc_command)
			elif annotation.startswith("af+"):	
				af = annotation.split(" ")
				address = af[1]
				fname = "_".join(af[3:]).strip().replace("/","_").replace("-","_")
				idc_command = 'idc.MakeName(%s, "%s")'%(address,fname)
				idc_commands.append(idc_command)
			elif annotation.startswith("f "):
				flag = annotation.split(" ")
				name = flag[1]
				address = flag[3].strip()
				idc_command = 'idc.MakeComm(%s,"%s")'%(address,name)
				idc_commands.append(idc_command)
				idc_command = 'idc.MarkPosition(%s,0,0,0,%d,"%s")'%(address,marks,name)
				idc_commands.append(idc_command)
				marks+=1

	for cmd in idc_commands:
		print cmd

if __name__ == '__main__':
	main()
