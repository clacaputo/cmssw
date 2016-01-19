import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.Geometry.GeometryExtended2023MuonReco_cff')
process.load('Configuration.Geometry.GeometryExtended2023Muon_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.load("DQMServices.Components.MEtoEDMConverter_cfi")
process.load("DQMServices.Core.DQM_cfg")

## global tag for 2019 upgrade studies
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:upgradePLS3', '')

process.load("Validation.MuonME0Hits.MuonME0Hits_cfi")
process.me0HitsValidation.EffSaveRootFile = True
process.load("Validation.MuonME0Digis.MuonME0Digis_cfi")
process.me0DigiValidation.EffSaveRootFile = True
process.load("Validation.MuonME0RecHits.MuonME0RecHits_cfi")
process.me0RecHitsValidation.EffSaveRootFile = True

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(  'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_1.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_10.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_100.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_101.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_102.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_103.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_104.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_105.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_106.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_107.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_108.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_109.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_11.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_110.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_111.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_112.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_113.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_114.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_115.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_116.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_117.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_118.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_119.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_12.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_120.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_121.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_122.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_123.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_124.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_125.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_126.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_127.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_128.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_129.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_13.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_130.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_131.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_132.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_133.root',
'file:/lustre/cms/store/user/piet/ME0Segment_Time/SampleProd4/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_GEN-SIM_v2/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola_HGCALGS_PU140_1ns_500um_1cm_SP4_v1_DIGI/151210_173525/0000/JME-TP2023SHCALDR-00001_step1_134.root'
 )
)
process.p = cms.Path(#process.me0HitsValidation *
		     process.me0DigiValidation
		     #process.me0RecHitsValidation
			)
