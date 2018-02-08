//
// Created by Patrick Yao on 07/02/2018.
//

#include <finder/finder.h>
#include <stopwatch/stopwatch.h>
#include <ectlrf/blif_netlist.h>
#include <iostream>

using namespace std;
using namespace ECTL::Utils;

static Path CircuitsPath =
        Path(RESOURCE_BASE_DIR)
                .sub(CIRCUIT_BASE_DIR)
                .sub("benchmark");

int main() {
    cout << CircuitsPath.toString() << endl;
    cout << (CircuitsPath / fBlif("C880")) << endl;

    ECTL::BlifNetlist netlist{};

    netlist.loadFromBlif(CircuitsPath / fBlif("C880"));

    for (auto gen = netlist.Inputs().generator(); !gen.hasEnded();)
        cout << gen.next().name() << endl;

    for (auto gen = netlist.Outputs().generator(); !gen.hasEnded();)
        cout << gen.next().name() << endl;

}