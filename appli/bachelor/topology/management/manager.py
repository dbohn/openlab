
from iotlabaggregator import serial
from iotlabcli.parser import common as common_parser

import line
from GraphRenderer import GraphvizRenderer
import time

def opts_parser():
    """ Argument parser object """
    import argparse
    parser = argparse.ArgumentParser()
    common_parser.add_auth_arguments(parser)

    nodes_group = parser.add_argument_group(
        description="By default, select currently running experiment nodes",
        title="Nodes selection")

    nodes_group.add_argument('-i', '--id', dest='experiment_id', type=int,
                             help='experiment id submission')

    nodes_group.add_argument(
        '-l', '--list', type=common_parser.nodes_list_from_str,
        dest='nodes_list', help='nodes list, may be given multiple times')

    nodes_group.add_argument(
        '-o', '--out-file',
        dest='outfile', help='Files where to output traces')

    return parser

def main():
    parser = opts_parser()
    opts = parser.parse_args()
    opts.with_a8 = False # needed, as we are not using a8 nodes
    # Load the selected nodes
    nodes_list = serial.SerialAggregator.select_nodes(opts)
    print "Starting manager..."
    print "Gestartete Knoten: %s" %(", ".join(nodes_list))
    receiver = line.Receiver()

    with serial.SerialAggregator(nodes_list, print_lines=False, line_handler=receiver.parse_line) as aggregator:
        # Issue neighbourhood lookup for each node
        for node in nodes_list:
            print "Topology search for %s" %(node)
            aggregator.send_nodes([node], "t")
            time.sleep(2)

        # Aggregate neighbourhood
        aggregator.send_nodes(None, "l")
        time.sleep(3)
        renderer = GraphvizRenderer()
        renderer.render(receiver.mergeLinks())

if __name__ == '__main__':
    main()