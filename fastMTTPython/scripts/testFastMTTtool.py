#Andrwe Loeliger
#quick script for testing the operation of the (hopefully) simplifiying fast mtt tool

from bbtautauAnalysisScripts.fastMTTPython.fastMTTtool import *

exampleMET = fastMTTmet(
    measuredX = 11.7491,
    measuredY = -51.9172,
    xx = 787.352,
    xy = -178.63,
    yy = 179.545,
)

firstLepton = fastMTTlepton(
    pt = 33.7383,
    eta = 0.9409,
    phi = -0.541458,
    m = 0.511E-3,
    leptonType = 'e'
)

secondLepton = fastMTTlepton(
    pt = 25.7322,
    eta = 0.618228,
    phi = 2.79362,
    m = 0.13957,
    leptonType = 'Tau'
)

theFastMTTtool = fastMTTtool()
theFastMTTtool.setFirstLepton(firstLepton)
theFastMTTtool.setSecondLepton(secondLepton)
theFastMTTtool.setTheMET(exampleMET)

print theFastMTTtool.getFastMTTmass()
