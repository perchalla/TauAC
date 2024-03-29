#include "../interface/ACFittedThreeProngDecay.h"

ClassImp(ACFittedThreeProngDecay)

ACFittedThreeProngDecay::ACFittedThreeProngDecay():ACFittedDecay() {
    constraints_ = -1;
    ndf_ = -1;
    
    vtxSignPVRotSV_ = -1.;
    vtxSignPVRotPVRed_ = -1.;
    a1Mass_ = -1.;
    energyTFraction_ = -1.;
}
ACFittedThreeProngDecay::ACFittedThreeProngDecay(std::vector<ACFitParticleRef> particles, std::string name, int iterations, float csum, int maxiterations, float mincsum, float chi2, ACPFTauRef PFTauRef, int constraints, int ndf):
ACFittedDecay(particles, name, iterations, csum, maxiterations, mincsum, chi2, PFTauRef) {
    constraints_ = constraints;
    ndf_ = ndf;

    vtxSignPVRotSV_ = -1.;
    vtxSignPVRotPVRed_ = -1.;
    a1Mass_ = -1.;
    energyTFraction_ = -1.;
}

const int ACFittedThreeProngDecay::ndf() const {
    return ndf_;
}
const int ACFittedThreeProngDecay::constraints() const { return constraints_;}
const float ACFittedThreeProngDecay::normalizedChi2() const {
    if (!testDecay()) return -1.;
    return (ndf()==0) ? -1 : (float)chi2() / (float)ndf();
}
const double ACFittedThreeProngDecay::chi2Probability() const {
    return TMath::Prob(chi2(), (int)ndf());
}

const ACFitParticle * ACFittedThreeProngDecay::tau() const { if (testDecay()) return particles()->at(0).get(); return 0; }
const ACFitParticle * ACFittedThreeProngDecay::prong0() const { if (testDecay()) return particles()->at(1).get(); return 0; }
const ACFitParticle * ACFittedThreeProngDecay::prong1() const { if (testDecay()) return particles()->at(2).get(); return 0; }
const ACFitParticle * ACFittedThreeProngDecay::prong2() const { if (testDecay()) return particles()->at(3).get(); return 0; }
const ACFitParticle * ACFittedThreeProngDecay::nu() const { if (testDecay()) return particles()->at(4).get(); return 0; }

const TLorentzVector ACFittedThreeProngDecay::a1Lorentz(bool prefit) const {
    if (!testDecay()) return TLorentzVector(0,0,0,0);
    TLorentzVector sum;
    for (std::vector<ACFitParticleRef>::const_iterator p = particles()->begin(); p != particles()->end(); ++p) {
        if (p == particles()->begin()) continue; // ignore tau
        if ((*p)->charge()==0.) continue; // ignore nu
        if (!prefit) { // default
            sum += (*p)->p4();
        } else {
            sum += (*p)->initialP4();
        }
    }

    return sum;
}
const double ACFittedThreeProngDecay::gjAngle(bool prefit) const {
    if (!testDecay()) return -1.;
    return a1Lorentz(prefit).Angle(tau()->p4().Vect());
}
const double ACFittedThreeProngDecay::maxGJAngle(bool prefit) const {
    if (!testDecay()) return -1.;
    TLorentzVector a1 = a1Lorentz(prefit);
    double Mtau = 1.777;
    double argument = (-pow(a1.M(),2.) + pow(Mtau,2.))/(2.*Mtau*a1.P());
    if (fabs(argument) >  1.0) {
//        printf("ACFittedThreeProngDecay::maxGJAngle: Warning! arcsin(%f) = %f. (pa1 %f, ma1 %f)\n", argument, asin(argument), a1.P(), a1.M());
        if (argument >  1.0) argument =  1.0;
        if (argument < -1.0) argument = -1.0;
    }

    return asin(argument);
}

bool ACFittedThreeProngDecay::testDecay() const {
    if (numberOfParticles()!=5) {
        printf("ACFittedThreeProngDecay::testDecay: Bad decay assumption of 5 particles. Found instead %d particles.\n", numberOfParticles());
        return false;
    }
    return true;
}

const int ACFittedThreeProngDecay::sgnlConeTrkSize() const {
    if (PFTauRef().isValid()) {
        return PFTauRef()->signalPFChargedHadrCands();
    }
    printf("ACFittedThreeProngDecay::sgnlConeTrkSize:ERROR! reference to PFTau invalid. Return dummy value.\n");
    return -1;
}
