#include "helpers.hh"

/// make all long-lived B-hadrons stable
void set_bflavour_stable(Pythia8::Pythia & pythia) {
    set_stable(pythia, 5);

    set_stable(pythia, 511);
    set_stable(pythia, 521);
    set_stable(pythia, 531);
    set_stable(pythia, 541);
    set_stable(pythia, 551);

    set_stable(pythia, 5122);
    set_stable(pythia, 5132);
    set_stable(pythia, 5142);
    set_stable(pythia, 5232);
    set_stable(pythia, 5242);
    set_stable(pythia, 5332);
    set_stable(pythia, 5342);
    set_stable(pythia, 5412);
    set_stable(pythia, 5414);
    set_stable(pythia, 5422);
    set_stable(pythia, 5424);
    set_stable(pythia, 5432);
    set_stable(pythia, 5434);
    set_stable(pythia, 5442);
    set_stable(pythia, 5444);
    set_stable(pythia, 5514);
    set_stable(pythia, 5522);
    set_stable(pythia, 5524);
    set_stable(pythia, 5532);
    set_stable(pythia, 5534);
    set_stable(pythia, 5542);
    set_stable(pythia, 5544);
    set_stable(pythia, 5554);

}
