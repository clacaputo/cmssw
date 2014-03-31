import FWCore.ParameterSet.Config as cms
from Validation.MuonGEMRecHits.simTrackMatching_cfi import SimTrackMatching

gemRecHitsValidation = cms.EDAnalyzer("MuonGEMRecHits",
                                      debug = cms.untracked.bool(False),
                                      AnalyzeTracks = cms.untracked.bool(True),
                                      folderPath = cms.untracked.string('MuonGEMRecHitsV/GEMRecHitTask'),
                                      EffSaveRootFile = cms.untracked.bool(True),
                                      EffRootFileName = cms.untracked.string('GEMRecHit_ME.root'),
                                      simTrackMatching = SimTrackMatching
)
