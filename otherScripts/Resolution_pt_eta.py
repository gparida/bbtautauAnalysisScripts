from ROOT import *
import math 
import time
from tqdm import tqdm

def getGenTauVisPt(theTree, genTauIndex):
        daughterParticles = []
        for nPart in range(theTree.nGenPart):
                if (theTree.GenPart_genPartIdxMother[nPart] == genTauIndex
                    and abs(theTree.GenPart_pdgId[nPart]) != 12
                    and abs(theTree.GenPart_pdgId[nPart]) != 14
                    and abs(theTree.GenPart_pdgId[nPart]) != 16):
                        theParticle = TLorentzVector()
                        theParticle.SetPtEtaPhiM(theTree.GenPart_pt[nPart],
                                                 theTree.GenPart_eta[nPart],
                                                 theTree.GenPart_phi[nPart],
                                                 theTree.GenPart_mass[nPart])
                        daughterParticles.append(theParticle)
        #now reconstruct the entire visible vector and return it's pt
        visibleTauVector = TLorentzVector()
        for particle in daughterParticles:
                visibleTauVector += particle
        return visibleTauVector.Pt()
                


hist_bbtt_pt_ratio = TH1F("hist_bbtt_pt_ratio","hist_bbtt_pt_ratio", 40, 0, 2)
hist_bbtt_pt_ratio_b = TH1F("hist_bbtt_pt_ratio_b","hist_bbtt_pt_ratio_b", 40, 0, 2)
hist_bbtt_eta_ratio = TH1F("hist_bbtt_eta_ratio","hist_bbtt_eta_ratio", 80, 0.95, 1.05)
hist_bbtt_eta_ratio_b = TH1F("hist_bbtt_eta_ratio_b","hist_bbtt_eta_ratio_b", 80, 0.95, 1.05)

#hist_bbtt_higgspt_total = TH1F("Hist_higgspt_total","Hist_higgspt_total",40, 0, 2000)



tau_pdg = 15
higgs_pdg = 25
tau_status = 2
tau_id = 2

tau_selected = 0
tau_passed = 0
tau_passed_b = 0



Tau_gen_v1=TLorentzVector(0.0,0.0,0.0,0.0)
Tau_reco_v2=TLorentzVector(0.0,0.0,0.0,0.0)

file_bbtt={} 
samplesFiles = ['New_NanoAOD_M1000','New_NanoAOD_M2000','New_NanoAOD_M4000']

for samples in samplesFiles:
	file_bbtt[samples] = TFile(samples+".root")
	tree_bbtt = file_bbtt[samples].Get('Events')
	nEntries = tree_bbtt.GetEntries()
	print nEntries
	
	
	i_counter=0
	delta_r=0
	delta_r_least=0
	i_least=0
	
	
	
	
	
	######M4000###########
	
	
	###########M2000##########
	#hist_bbtt_higgspt_pass = TH1F("Hist_higgspt_pass","Hist_higgspt_pass", 15, 100, 1500)
	#hist_bbtt_higgspt_pass_b = TH1F("Hist_higgspt_pass_b","Hist_higgspt_pass_b", 15, 100, 1500)
	#hist_bbtt_higgspt_total = TH1F("Hist_higgspt_total","Hist_higgspt_total",15, 100, 1500)
	
	#hist_bbtt_higgspt_pass = TH1F("Hist_higgspt_pass","Hist_higgspt_pass", 15, 0, 1100)
	#hist_bbtt_higgspt_pass_b = TH1F("Hist_higgspt_pass_b","Hist_higgspt_pass_b", 15, 0, 1100)
	#hist_bbtt_higgspt_total = TH1F("Hist_higgspt_total","Hist_higgspt_total",15, 0, 1100)
	
	
	
	for x in tqdm(range(nEntries)):
		#if (x%1000 == 0):
			#print("events_processed=",x)
		tree_bbtt.GetEntry(x)
	
	
		for i in range(len(tree_bbtt.GenPart_pdgId)):
			i_counter=0
			i_least=0
			delta_r=0
			delta_r_least=0
			if(abs(tree_bbtt.GenPart_pdgId[i])!= tau_pdg):
				continue
			if(tree_bbtt.GenPart_statusFlags[i] & tau_status != tau_status):
				continue
                        if (tree_bbtt.GenPart_genPartIdxMother[i] < 0):
                                continue
			if(tree_bbtt.GenPart_pdgId[tree_bbtt.GenPart_genPartIdxMother[i]]!=higgs_pdg):
				continue
	
			tau_selected = tau_selected +1 
			#hist_bbtt_higgspt_total.Fill(tree_bbtt.GenPart_pt[tree_bbtt.GenPart_genPartIdxMother[i]])
			Tau_gen_v1.SetPtEtaPhiM(tree_bbtt.GenPart_pt[i],tree_bbtt.GenPart_eta[i],tree_bbtt.GenPart_phi[i],tree_bbtt.GenPart_mass[i])
                        #print('***********')
                        #print(getGenTauVisPt(tree_bbtt,i))
                        #print(Tau_gen_v1.Pt())
                        #print('***********')
                        Tau_gen_v1.SetPtEtaPhiM(getGenTauVisPt(tree_bbtt,i),tree_bbtt.GenPart_eta[i],tree_bbtt.GenPart_phi[i],tree_bbtt.GenPart_mass[i])
	
			for j in range(len(tree_bbtt.Tau_pt)):
				Tau_reco_v2.SetPtEtaPhiM(tree_bbtt.Tau_pt[j],tree_bbtt.Tau_eta[j],tree_bbtt.Tau_phi[j],tree_bbtt.Tau_mass[j])
				delta_r=Tau_gen_v1.DeltaR(Tau_reco_v2)
				if (i_counter==0 or delta_r<delta_r_least):
					delta_r_least=delta_r
					i_least=j
					i_counter=i_counter+1
	
	
			if(delta_r_least < 0.1 and i_counter!=0):
				tau_passed = tau_passed + 1
				hist_bbtt_pt_ratio.Fill(tree_bbtt.Tau_pt[i_least]/getGenTauVisPt(tree_bbtt,i))
				hist_bbtt_eta_ratio.Fill(tree_bbtt.Tau_eta[i_least]/tree_bbtt.GenPart_eta[i])
	
	
			i_counter=0
			i_least=0
			delta_r=0
			delta_r_least=0
	
			for j in range(len(tree_bbtt.boostedTau_pt)):
				Tau_reco_v2.SetPtEtaPhiM(tree_bbtt.boostedTau_pt[j],tree_bbtt.boostedTau_eta[j],tree_bbtt.boostedTau_phi[j],tree_bbtt.boostedTau_mass[j])
				delta_r=Tau_gen_v1.DeltaR(Tau_reco_v2)
				if (i_counter==0 or delta_r<delta_r_least):
					delta_r_least=delta_r
					i_least=j
					i_counter=i_counter+1
	
	
			if(delta_r_least < 0.1 and i_counter!=0):
				hist_bbtt_pt_ratio_b.Fill(tree_bbtt.boostedTau_pt[i_least]/getGenTauVisPt(tree_bbtt,i))
				hist_bbtt_eta_ratio_b.Fill(tree_bbtt.boostedTau_eta[i_least]/tree_bbtt.GenPart_eta[i])
	
	
			i_counter=0
			i_least=0
			delta_r=0
			delta_r_least=0
	


#hist_bbtt_higgspt_eff = TGraphAsymmErrors(hist_bbtt_higgspt_pass,hist_bbtt_higgspt_total,'e1000')
#hist_bbtt_higgspt_eff_b = TGraphAsymmErrors(hist_bbtt_higgspt_pass_b,hist_bbtt_higgspt_total,'e1000_b')	

hist_bbtt_pt_ratio.Scale(1.0/hist_bbtt_pt_ratio.Integral())
hist_bbtt_pt_ratio_b.Scale(1.0/hist_bbtt_pt_ratio_b.Integral())
hist_bbtt_eta_ratio.Scale(1.0/hist_bbtt_eta_ratio.Integral())
hist_bbtt_eta_ratio_b.Scale(1.0/hist_bbtt_eta_ratio_b.Integral())

hist_bbtt_pt_ratio.SetLineColor(1)
hist_bbtt_pt_ratio.SetMarkerStyle(20)
hist_bbtt_pt_ratio.SetMarkerColor(1)
hist_bbtt_pt_ratio.SetMarkerSize(1.5)
hist_bbtt_pt_ratio.SetTitle("Tau p_{T} Resolution")
hist_bbtt_pt_ratio.GetXaxis().SetTitle("Reconstructed p_{T}/Gen p_{T}")
hist_bbtt_pt_ratio.GetYaxis().SetTitle("Fraction")
hist_bbtt_pt_ratio.SetStats(0)

#hist_bbtt_pt_ratio.GetYaxis().SetRangeUser(-0.05,1.1)
	
hist_bbtt_pt_ratio_b.SetLineColor(2)
hist_bbtt_pt_ratio_b.SetMarkerStyle(21)
hist_bbtt_pt_ratio_b.SetMarkerColor(2)
hist_bbtt_pt_ratio_b.SetMarkerSize(1.5)
hist_bbtt_pt_ratio_b.SetTitle("Tau p_{T} Resolution")
hist_bbtt_pt_ratio_b.GetXaxis().SetTitle("Reconstructed p_{T}/Gen p_{T}")
hist_bbtt_pt_ratio_b.GetYaxis().SetTitle("Fraction")
hist_bbtt_pt_ratio_b.SetStats(0)

hist_bbtt_eta_ratio.SetLineColor(1)
hist_bbtt_eta_ratio.SetMarkerStyle(20)
hist_bbtt_eta_ratio.SetMarkerColor(1)
hist_bbtt_eta_ratio.SetMarkerSize(1.2)
hist_bbtt_eta_ratio.SetTitle("Tau #eta Resolution")
hist_bbtt_eta_ratio.GetXaxis().SetTitle("Reconstructed #eta/Gen #eta")
hist_bbtt_eta_ratio.GetYaxis().SetTitle("Fraction")
hist_bbtt_eta_ratio.SetStats(0)
#hist_bbtt_eta_ratio.GetYaxis().SetRangeUser(-0.05,1.1)
	
hist_bbtt_eta_ratio_b.SetLineColor(2)
hist_bbtt_eta_ratio_b.SetMarkerStyle(21)
hist_bbtt_eta_ratio_b.SetMarkerColor(2)
hist_bbtt_eta_ratio_b.SetMarkerSize(1.2)
hist_bbtt_eta_ratio_b.SetTitle("Tau #eta Resolution")
hist_bbtt_eta_ratio_b.GetXaxis().SetTitle("Reconstructed #eta/Gen #eta")
hist_bbtt_eta_ratio_b.GetYaxis().SetTitle("Fraction")
hist_bbtt_eta_ratio_b.SetStats(0)
#hist_bbtt_eta_ratio_b.GetYaxis().SetRangeUser(-0.05,1.1)
	
print ("non_b_taus",tau_selected, tau_passed)
print ("b_taus",tau_selected, tau_passed_b)	


legend1 = TLegend(0.1289398,0.6281513,0.45,0.8802521)
legend1.SetFillStyle(1001)
legend1.AddEntry(hist_bbtt_pt_ratio,"standard Reconstruction","ep")
legend1.AddEntry(hist_bbtt_pt_ratio_b,"boosted Reconstruction","ep")
#legend1.SetBorderSize(0)

can1 = TCanvas("canvas1", "efficiency")
#can1.cd()
can1.SetGrid()
#can1 = TCanvas("canvas", "Trigger Efficiency")
hist_bbtt_pt_ratio_b.Draw("HIST P")
hist_bbtt_pt_ratio.Draw("SAME")
#hist_bbtt_higgspt_eff_2000.Draw("ap")
#hist_bbtt_higgspt_eff_2000_b.Draw("same p")
#hist_bbtt_higgspt_eff_4000.Draw("ap")
#hist_bbtt_higgspt_eff_4000_b.Draw("same p")
#can1 = TCanvas("canvas", "Trigger Efficiency")
#hist_bbtt_higgspt_eff.Draw("same p")
#hist_bbtt_higgspt_eff_b.Draw("same p")
#hist_bbtt_higgspt_eff_2000.Draw("same p")
#hist_bbtt_higgspt_eff_2000_b.Draw("same p")
#l.Draw("same")	
legend1.Draw("same")
cmsLatex = TLatex()
cmsLatex.SetTextSize(0.04)
cmsLatex.SetNDC(True)
cmsLatex.SetTextFont(61)
cmsLatex.SetTextAlign(11)
cmsLatex.DrawLatex(0.1,0.92,"CMS")
cmsLatex.SetTextFont(52)
cmsLatex.DrawLatex(0.1+0.07,0.92,"Preliminary")
can1.SaveAs('Resolution_one.png')
can1.SaveAs('Resolution_one.pdf')

legend2 = TLegend(0.1289398,0.6281513,0.45,0.8802521)
#legend.SetHeader("#tau_{h}-#tau_{h} channels","C")
legend2.SetFillStyle(1001)
legend2.AddEntry(hist_bbtt_eta_ratio,"standard Reconstruction","ep")
legend2.AddEntry(hist_bbtt_eta_ratio_b,"boosted Reconstruction","ep")
#legend2.SetBorderSize(0)

can2 = TCanvas("canvas2", "efficiency")
#can1.cd()
can2.SetGrid()
#can1 = TCanvas("canvas", "Trigger Efficiency")
hist_bbtt_eta_ratio_b.Draw("HIST P")
hist_bbtt_eta_ratio.Draw("SAME")
#hist_bbtt_higgspt_eff_2000.Draw("ap")
#hist_bbtt_higgspt_eff_2000_b.Draw("same p")
#hist_bbtt_higgspt_eff_4000.Draw("ap")
#hist_bbtt_higgspt_eff_4000_b.Draw("same p")
#can1 = TCanvas("canvas", "Trigger Efficiency")
#hist_bbtt_higgspt_eff.Draw("same p")
#hist_bbtt_higgspt_eff_b.Draw("same p")
#hist_bbtt_higgspt_eff_2000.Draw("same p")
#hist_bbtt_higgspt_eff_2000_b.Draw("same p")

#l.Draw("same")	
legend2.Draw("same")
cmsLatex = TLatex()
cmsLatex.SetTextSize(0.04)
cmsLatex.SetNDC(True)
cmsLatex.SetTextFont(61)
cmsLatex.SetTextAlign(11)
cmsLatex.DrawLatex(0.1,0.92,"CMS")
cmsLatex.SetTextFont(52)
cmsLatex.DrawLatex(0.1+0.07,0.92,"Preliminary")

can2.SaveAs('Resolution_two.png')
can2.SaveAs('Resolution_two.pdf')

#time.sleep(1000)
raw_input("Press Enter to continue...")
