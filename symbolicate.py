#!/Library/Frameworks/Python.framework/Versions/3.7/bin/python3.7
import sys
def parse_map(file):
	result = {}
	inmode = False
	submode = False
	for line in file:
		if line[0] == '.':
			submode = False
			was = inmode
			inmode = line.startswith('.text')
			if was and not inmode:
				address = int([x for x in line.split(' ') if x][1], 16)
				result[address] = None
			continue
		if not inmode:
			continue
		line = line.rstrip()
		parts = line.split(' ')
		if len(parts) >= 2 and (parts[1] == '.text' or parts[1] == '*fill*'):
			submode = parts[1] == '.text'
			continue
		if not submode:
			continue
		parts = [x for x in parts if x]
		if len(parts) != 2:
			continue
		address = int(parts[0], 16)
		name = parts[1]
		result[address] = name
	return result
def load(param):
	bits = param.split(':', 1)
	return (bits[0], parse_map(open(bits[1], 'rt')))
symbols = [load(x) for x in sys.argv[1:]]

def map_address(addr):
	for b,l in symbols:
		last = None
		for a in l.keys():
			if a > addr:
				break
			last = a
		if last is not None and l[last] is not None:
			return f"{b}-{l[last]}"
	return f"0x{addr:08x}"

for line in sys.stdin:
	parts = [x for x in line.rstrip().split(' ') if x]
	if len(parts) < 2:
		continue
	count = int(parts[0])
	res = []
	for address in parts[1].split(','):
		raw = int(address, 16)
		res.append(map_address(raw))
	print(f"{count:4} {','.join(res)}")

