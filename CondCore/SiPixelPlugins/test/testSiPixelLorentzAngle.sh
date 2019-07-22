#!/bin/bash
# Save current working dir so img can be outputted there later
W_DIR=$(pwd);
# Set SCRAM architecture var
SCRAM_ARCH=slc6_amd64_gcc630;
export SCRAM_ARCH;
source /afs/cern.ch/cms/cmsset_default.sh;
eval `scram run -sh`;
# Go back to original working directory
cd $W_DIR;
# Run get payload data script

getPayloadData.py \
    --plugin pluginSiPixelLorentzAngle_PayloadInspector \
    --plot plot_SiPixelLorentzAngleValueComparisonTwoTags \
    --tag SiPixelLorentzAngleSim_phase1_BoR3_HV350_Tr1300 \
    --tagtwo SiPixelLorentzAngle_phase1_BoR3_HV350_Tr1300 \
    --time_type Run \
    --iovs '{"start_iov": "1", "end_iov": "1"}' \
    --iovstwo '{"start_iov": "1", "end_iov": "1"}' \
    --db Prod \
    --test ;

getPayloadData.py \
    --plugin pluginSiPixelLorentzAngle_PayloadInspector \
    --plot plot_SiPixelLorentzAngleValues \
    --tag SiPixelLorentzAngle_forWidth_phase1_mc_v1 \
    --time_type Run \
    --iovs '{"start_iov": "1", "end_iov": "1"}' \
    --db Prod \
    --test ;                                  
