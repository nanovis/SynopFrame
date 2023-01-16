import argparse
from os import path


def cli_parser(prog="hb_design2forces.py"):
    parser = argparse.ArgumentParser(prog=prog, description="convert hb design to external forces file")
    parser.add_argument("hb_design", type=str, nargs=1, help="hb design file")
    parser.add_argument("-o", metavar="out", dest="out", type=str, nargs=1, help="out file name, defualt to external_forces.txt")
    parser.add_argument("-s", metavar="stiff", nargs=1, type=float, dest="stiff", help="stiffness of the mutual trap, default to 0.09")
    return parser


def get_mutual_force(id1, id2, stiffness=0.1):
    base_str = "{\n    type = mutual_trap\n    particle = %d\n    ref_particle = %d\n    stiff = %.3f \n    r0 = 1.2 \n    PBC = 1\n}\n\n"
    s = base_str % (id1, id2, stiffness)
    s = s + base_str % (id2, id1, stiffness)
    return s


def main():
    parser = cli_parser(path.basename(__file__))
    args = parser.parse_args()
    hb_design_file = args.hb_design[0]
    stiff = args.stiff
    out = args.out

    if not stiff:
        stiff = 0.09

    if not out:
        out = "external_forces.txt"

    with open(hb_design_file, 'rt') as hb_handle:
        with open(out, 'wt') as out_handle:
        
            hb_handle.readline()
            line = hb_handle.readline()
            while line:
                tokens = line.split()
                nt1 = int(tokens[0])
                nt2 = int(tokens[1])
                
                out_handle.write(get_mutual_force(nt1, nt2, stiff))

                line = hb_handle.readline()


                
if __name__ == '__main__':
    main()

