from ROOT import *
import math 
import time
from plot_dict import *
from tqdm import tqdm

def classifyTauDecayMode(theTree):
        particlesWithMotherTau = []
        #find particles who had a mother tau
        for nPart in range(theTree.nGenPart):
                if theTree.GenPart_genPartIdxMother[nPart] < 0:
                        continue
                if (abs(theTree.GenPart_pdgId[theTree.GenPart_genPartIdxMother[nPart]]) == 15
                    and theTree.GenPart_statusFlags[theTree.GenPart_genPartIdxMother[nPart]] & 3 == 3): #tau mother, record the pdg ID
                        particlesWithMotherTau.append((nPart,theTree.GenPart_pdgId[nPart],theTree.GenPart_genPartIdxMother[nPart]))
        #okay, now search the particles with mother taus and classify what we have
        decayMode = []
        alreadyAssignedTaus = []
        for particle in particlesWithMotherTau:
                if particle[2] not in alreadyAssignedTaus:
                        if abs(particle[1]) == 13:
                                decayMode.append('m')
                                alreadyAssignedTaus.append(particle[2])
                        if abs(particle[1]) == 11:
                                decayMode.append('e')
                                alreadyAssignedTaus.append(particle[2])
                        if abs(particle[1]) == 111 or abs(particle[1]) == 211:
                                decayMode.append('t')
                                alreadyAssignedTaus.append(particle[2])
        if len(decayMode) < 2:
                #assume we decayed to some other hadron
                for i in range (2-len(decayMode)):
                        decayMode.append('t')
        if len(decayMode) > 2:
                print('too many assigned taus!')
                for tau in alreadyAssignedTaus:
                        print('{:3n}'.format(tau)+': '+'{0:b}'.format(theTree.GenPart_statusFlags[tau]))
                print('returning none!')
                return None
        if len(decayMode) < 2:
                print('too few taus!')
                for tau in particlesWithMotherTau:
                        if tau[2] not in alreadyAssignedTaus:
                                print("{:3n}".format(tau[1])+' Was part of a tau that was not assigned')
                return None
        decayMode.sort()
        return ''.join(decay for decay in decayMode)
                

#hist_bbtt_higgspt_pass = TH1F("Hist_higgspt_pass","Hist_higgspt_pass", 40, 0, 2000)
#hist_bbtt_higgspt_pass_b = TH1F("Hist_higgspt_pass_b","Hist_higgspt_pass_b", 40, 0, 2000)
#hist_bbtt_higgspt_total = TH1F("Hist_higgspt_total","Hist_higgspt_total",40, 0, 2000)


#print (markercolor[2])
tau_pdg = 15
higgs_pdg = 25
tau_status = 2
#tau_id = 2
hist_bbtt_higgspt_pass = TH1F("Hist_higgspt_pass","Hist_higgspt_pass", 13, 300, 1000)
hist_bbtt_higgspt_pass_b = TH1F("Hist_higgspt_pass_b","Hist_higgspt_pass_b", 13, 300, 1000)
hist_bbtt_higgspt_total = TH1F("Hist_higgspt_total","Hist_higgspt_total",13, 300, 1000)

hist_bbtt_higgspt_pass_ttchannel = TH1F("Hist_higgspt_pass_ttchannel","Hist_higgspt_pass_ttchannel", 13, 300, 1000)
hist_bbtt_higgspt_pass_b_ttchannel = TH1F("Hist_higgspt_pass_b_ttchannel","Hist_higgspt_pass_b_ttchannel", 13, 300, 1000)
hist_bbtt_higgspt_total_ttchannel = TH1F("Hist_higgspt_total_ttchannel","Hist_higgspt_total_ttchannel",13, 300, 1000)



samplesFiles = ['New_NanoAOD_M1000','New_NanoAOD_M2000','New_NanoAOD_M4000']
tau_working =[1]
#hist_bbtt_higgspt_pass=['hist_bbtt_higgspt_pass','hist_bbtt_higgspt_pass_2','hist_bbtt_higgspt_pass_4','hist_bbtt_higgspt_pass_8','hist_bbtt_higgspt_pass_16','hist_bbtt_higgspt_pass_32','hist_bbtt_higgspt_pass_64']
#hist_bbtt_higgspt_pass_b=['hist_bbtt_higgspt_pass_b','hist_bbtt_higgspt_pass_2_b','hist_bbtt_higgspt_pass_4_b','hist_bbtt_higgspt_pass_8_b','hist_bbtt_higgspt_pass_16_b','hist_bbtt_higgspt_pass_32_b','hist_bbtt_higgspt_pass_64_b']
#hist_bbtt_higgspt_total =['hist_bbtt_higgspt_total','hist_bbtt_higgspt_total_2','hist_bbtt_higgspt_total_4','hist_bbtt_higgspt_total_8','hist_bbtt_higgspt_total_16','hist_bbtt_higgspt_total_32','hist_bbtt_higgspt_total_64']
#hist_bbtt_higgspt_eff=['hist_bbtt_higgspt_eff','hist_bbtt_higgspt_eff_2','hist_bbtt_higgspt_eff_4','hist_bbtt_higgspt_eff_8','hist_bbtt_higgspt_eff_16','hist_bbtt_higgspt_eff_32','hist_bbtt_higgspt_eff_64']
#hist_bbtt_higgspt_eff_b=['hist_bbtt_higgspt_eff_b','hist_bbtt_higgspt_eff_2_b','hist_bbtt_higgspt_eff_4_b','hist_bbtt_higgspt_eff_8_b','hist_bbtt_higgspt_eff_16_b','hist_bbtt_higgspt_eff_32_b','hist_bbtt_higgspt_eff_64_b']

hist_bbtt_higgspt_eff ={}
hist_bbtt_higgspt_eff_b={}
hist_bbtt_higgspt_eff_ttchannel ={}
hist_bbtt_higgspt_eff_b_ttchannel={}

Tau_gen_v1=TLorentzVector(0.0,0.0,0.0,0.0)
Tau_reco_v2=TLorentzVector(0.0,0.0,0.0,0.0)

for y in range(1):
	tau_selected = 0
	tau_passed = 0
	tau_passed_b = 0
	tau_id = tau_working[y]
	#print("ID Processing",tau_id)
	hist_bbtt_higgspt_pass.Reset()
	hist_bbtt_higgspt_pass_b.Reset()
	hist_bbtt_higgspt_total.Reset()
		
	for samples in samplesFiles:
		file_bbtt = TFile(samples+".root")
		tree_bbtt = file_bbtt.Get('Events')
		nEntries = tree_bbtt.GetEntries()
		#print nEntries
		
		
		i_counter=0
		delta_r=0
		delta_r_least=0
		i_least=0
	
	
		for x in tqdm(range(nEntries)):
			tree_bbtt.GetEntry(x)
                        decayMode = classifyTauDecayMode(tree_bbtt)
                        if decayMode == None: #something went wrong
                                continue
                        
		
			for i in range(len(tree_bbtt.GenPart_pdgId)):
				i_counter=0
				i_least=0
				delta_r=0
				delta_r_least=0
				if(abs(tree_bbtt.GenPart_pdgId[i])!= tau_pdg):
					continue
				if(tree_bbtt.GenPart_statusFlags[i] & tau_status != tau_status):
					continue
				if(tree_bbtt.GenPart_pdgId[tree_bbtt.GenPart_genPartIdxMother[i]]!=higgs_pdg):
					continue
		
				tau_selected = tau_selected +1 
				hist_bbtt_higgspt_total.Fill(tree_bbtt.GenPart_pt[tree_bbtt.GenPart_genPartIdxMother[i]])
                                if decayMode == 'tt':
                                        hist_bbtt_higgspt_total_ttchannel.Fill(tree_bbtt.GenPart_pt[tree_bbtt.GenPart_genPartIdxMother[i]])
				Tau_gen_v1.SetPtEtaPhiM(tree_bbtt.GenPart_pt[i],tree_bbtt.GenPart_eta[i],tree_bbtt.GenPart_phi[i],tree_bbtt.GenPart_mass[i])
		
				#print ("Entering Danger Zone")
				for j in range(len(tree_bbtt.Tau_pt)):
					#print ("Inside Danger Zone")
					Tau_reco_v2.SetPtEtaPhiM(tree_bbtt.Tau_pt[j],tree_bbtt.Tau_eta[j],tree_bbtt.Tau_phi[j],tree_bbtt.Tau_mass[j])
					delta_r=Tau_gen_v1.DeltaR(Tau_reco_v2)
					if (i_counter==0 or delta_r<delta_r_least):
						delta_r_least=delta_r
						i_least=j
						i_counter=i_counter+1
                                #print(type(tree_bbtt.Tau_idMVAoldDM2017v1))
                                #print(tree_bbtt.Tau_idMVAoldDM2017v1)
                                #print(tree_bbtt.Tau_idMVAoldDM2017v1[i_least] >= 1)
                                
                                if(tree_bbtt.nTau>0):
                                        if(delta_r_least < 0.1 
                                           and i_counter !=0 
                                           and tree_bbtt.Tau_pt[i_least]>20 
                                           and abs(tree_bbtt.Tau_eta[i_least])<2.3 
                                           and tree_bbtt.Tau_idDecayModeNewDMs[i_least]==1 
                                           and (int(''.join(format(ord(i), '08b') for i in tree_bbtt.Tau_idMVAoldDM2017v1[i_least])) & tau_id == tau_id)):
                                                tau_passed = tau_passed + 1
                                                hist_bbtt_higgspt_pass.Fill(tree_bbtt.GenPart_pt[tree_bbtt.GenPart_genPartIdxMother[i]])
                                                if decayMode == 'tt':
                                                        hist_bbtt_higgspt_pass_ttchannel.Fill(tree_bbtt.GenPart_pt[tree_bbtt.GenPart_genPartIdxMother[i]])
		
		
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
		
                                if(tree_bbtt.nboostedTau>0):
                                        if(delta_r_least < 0.1 
                                           and i_counter !=0 
                                           and tree_bbtt.boostedTau_pt[i_least]>20 
                                           and abs(tree_bbtt.boostedTau_eta[i_least])<2.3 
                                           and tree_bbtt.boostedTau_idDecayModeNewDMs[i_least]==1 
                                           and (int(''.join(format(ord(i), '08b') for i in tree_bbtt.boostedTau_idMVAoldDM2017v1[i_least])) & tau_id == tau_id)):
                                                tau_passed_b = tau_passed_b + 1
                                                hist_bbtt_higgspt_pass_b.Fill(tree_bbtt.GenPart_pt[tree_bbtt.GenPart_genPartIdxMother[i]])
                                                if decayMode == 'tt':
                                                        hist_bbtt_higgspt_pass_b_ttchannel.Fill(tree_bbtt.GenPart_pt[tree_bbtt.GenPart_genPartIdxMother[i]])
		
				i_counter=0
				i_least=0
				delta_r=0
				delta_r_least=0
	
	
	hist_bbtt_higgspt_eff[y] = TGraphAsymmErrors(hist_bbtt_higgspt_pass,hist_bbtt_higgspt_total,'e1000')
	hist_bbtt_higgspt_eff[y].SetLineColor(linecolor[y+1])
	hist_bbtt_higgspt_eff[y].SetMarkerStyle(markerstylesolid[y+1])
	hist_bbtt_higgspt_eff[y].SetMarkerColor(markercolor[y+1])
	hist_bbtt_higgspt_eff[y].SetMarkerSize(1.5)
	hist_bbtt_higgspt_eff[y].SetTitle("Reconstruction + Identification Efficiency")
	hist_bbtt_higgspt_eff[y].GetXaxis().SetTitle( "Gen Higgs p_{T}(GeV)")
	hist_bbtt_higgspt_eff[y].GetYaxis().SetTitle("Reconstruction + Identification Efficiency")
	hist_bbtt_higgspt_eff[y].GetYaxis().SetRangeUser(-0.05,1.1)




	hist_bbtt_higgspt_eff_b[y] = TGraphAsymmErrors(hist_bbtt_higgspt_pass_b,hist_bbtt_higgspt_total,'e1000_b')
	hist_bbtt_higgspt_eff_b[y].SetLineColor(linecolor[y+2])
	hist_bbtt_higgspt_eff_b[y].SetMarkerStyle(markerstylesolid[y+2])
	hist_bbtt_higgspt_eff_b[y].SetMarkerColor(markercolor[y+2])
	hist_bbtt_higgspt_eff_b[y].SetMarkerSize(1.5)
	hist_bbtt_higgspt_eff_b[y].SetTitle("Reconstruction + Identification Efficiency")
	hist_bbtt_higgspt_eff_b[y].GetXaxis().SetTitle("Gen Higgs p_{T}(GeV)")
	hist_bbtt_higgspt_eff_b[y].GetYaxis().SetTitle("Reconstruction + Identification Efficiency")
	hist_bbtt_higgspt_eff_b[y].GetYaxis().SetRangeUser(-0.05,1.1)

        

        hist_bbtt_higgspt_eff_ttchannel[y] = TGraphAsymmErrors(hist_bbtt_higgspt_pass_ttchannel,hist_bbtt_higgspt_total_ttchannel,'e1000_ttchannel')
	hist_bbtt_higgspt_eff_ttchannel[y].SetLineColor(linecolor[y+1])
	hist_bbtt_higgspt_eff_ttchannel[y].SetMarkerStyle(markerstylesolid[y+1])
	hist_bbtt_higgspt_eff_ttchannel[y].SetMarkerColor(markercolor[y+1])
	hist_bbtt_higgspt_eff_ttchannel[y].SetMarkerSize(1.5)
	hist_bbtt_higgspt_eff_ttchannel[y].SetTitle("Reconstruction + Identification Efficiency")
	hist_bbtt_higgspt_eff_ttchannel[y].GetXaxis().SetTitle( "Gen Higgs p_{T}(GeV)")
	hist_bbtt_higgspt_eff_ttchannel[y].GetYaxis().SetTitle("Reconstruction + Identification Efficiency")
	hist_bbtt_higgspt_eff_ttchannel[y].GetYaxis().SetRangeUser(-0.05,1.1)




	hist_bbtt_higgspt_eff_b_ttchannel[y] = TGraphAsymmErrors(hist_bbtt_higgspt_pass_b_ttchannel,hist_bbtt_higgspt_total_ttchannel,'e1000_b_ttchannel')
	hist_bbtt_higgspt_eff_b_ttchannel[y].SetLineColor(linecolor[y+2])
	hist_bbtt_higgspt_eff_b_ttchannel[y].SetMarkerStyle(markerstylesolid[y+2])
	hist_bbtt_higgspt_eff_b_ttchannel[y].SetMarkerColor(markercolor[y+2])
	hist_bbtt_higgspt_eff_b_ttchannel[y].SetMarkerSize(1.5)
	hist_bbtt_higgspt_eff_b_ttchannel[y].SetTitle("Reconstruction + Identification Efficiency")
	hist_bbtt_higgspt_eff_b_ttchannel[y].GetXaxis().SetTitle("Gen Higgs p_{T}(GeV)")
	hist_bbtt_higgspt_eff_b_ttchannel[y].GetYaxis().SetTitle("Reconstruction + Identification Efficiency")
	hist_bbtt_higgspt_eff_b_ttchannel[y].GetYaxis().SetRangeUser(-0.05,1.1)






can1 = TCanvas("canvas1", "efficiency")
#can1.cd()
can1.SetGrid()
#can1 = TCanvas("canvas", "Trigger Efficiency")
hist_bbtt_higgspt_eff[0].Draw("ap")
hist_bbtt_higgspt_eff_b[0].Draw("same p")
#hist_bbtt_higgspt_eff_b[0].Draw("same p")
#hist_bbtt_higgspt_eff[1].Draw("same p")
#hist_bbtt_higgspt_eff_b[1].Draw("same p")
#hist_bbtt_higgspt_eff[2].Draw("same p")
#hist_bbtt_higgspt_eff_b[2].Draw("same p")
#hist_bbtt_higgspt_eff[3].Draw("same p")
#hist_bbtt_higgspt_eff_b[3].Draw("same p")
#hist_bbtt_higgspt_eff[4].Draw("same p")
#hist_bbtt_higgspt_eff_b[4].Draw("same p")
#hist_bbtt_higgspt_eff[5].Draw("same p")
#hist_bbtt_higgspt_eff_b[5].Draw("same p")
#hist_bbtt_higgspt_eff[6].Draw("same p")
#hist_bbtt_higgspt_eff_b[6].Draw("same p")
#hist_bbtt_higgspt_eff_2000.Draw("ap")
#hist_bbtt_higgspt_eff_2000_b.Draw("same p")
#hist_bbtt_higgspt_eff_4000.Draw("ap")
#hist_bbtt_higgspt_eff_4000_b.Draw("same p")
#can1 = TCanvas("canvas", "Trigger Efficiency")
#hist_bbtt_higgspt_eff.Draw("same p")
#hist_bbtt_higgspt_eff_b.Draw("same p")
#hist_bbtt_higgspt_eff_2000.Draw("same p")
#hist_bbtt_higgspt_eff_2000_b.Draw("same p")
legend1 = TLegend(0.1289398,0.6281513,0.5100287,0.8802521)
#legend.SetHeader("#tau_{h}-#tau_{h} channels","C")
legend1.SetFillStyle(1001)
legend1.AddEntry(hist_bbtt_higgspt_eff[0],"standard_VVLoose","ep")
#legend.AddEntry(hist_bbtt_higgspt_eff_b[0],"boosted_VVLoose","ep")
legend1.AddEntry(hist_bbtt_higgspt_eff_b[0],"boosted_VVLoose","ep")
#legend.AddEntry(hist_bbtt_higgspt_eff_b[1],"boosted_VLoose","ep")
#legend1.AddEntry(hist_bbtt_higgspt_eff[2],"standard_Medium","ep")
#legend.AddEntry(hist_bbtt_higgspt_eff_b[2],"boosted_Loose","ep")
#legend1.AddEntry(hist_bbtt_higgspt_eff[3],"standard_Tight","ep")
#legend.AddEntry(hist_bbtt_higgspt_eff_b[3],"boosted_Medium","ep")
#legend1.AddEntry(hist_bbtt_higgspt_eff[4],"standard_Tight","ep")
#legend.AddEntry(hist_bbtt_higgspt_eff_b[4],"boosted_Tight","ep")
#legend1.AddEntry(hist_bbtt_higgspt_eff[5],"standard_VTight","ep")
#legend.AddEntry(hist_bbtt_higgspt_eff_b[5],"boosted_VTight","ep")
#legend1.AddEntry(hist_bbtt_higgspt_eff[6],"standard_VVTight","ep")

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



#can2 = TCanvas("canvas2", "efficiency")
##can1.cd()
#can2.SetGrid()
##can1 = TCanvas("canvas", "Trigger Efficiency")
#hist_bbtt_higgspt_eff_b[0].Draw("ap")
##hist_bbtt_higgspt_eff_b[0].Draw("same p")
#hist_bbtt_higgspt_eff_b[1].Draw("same p")
##hist_bbtt_higgspt_eff_b[1].Draw("same p")
#hist_bbtt_higgspt_eff_b[2].Draw("same p")
##hist_bbtt_higgspt_eff_b[2].Draw("same p")
#hist_bbtt_higgspt_eff_b[3].Draw("same p")
##hist_bbtt_higgspt_eff_b[3].Draw("same p")
##hist_bbtt_higgspt_eff_b[4].Draw("same p")
##hist_bbtt_higgspt_eff_b[4].Draw("same p")
##hist_bbtt_higgspt_eff_b[5].Draw("same p")
##hist_bbtt_higgspt_eff_b[5].Draw("same p")
##ist_bbtt_higgspt_eff_b[6].Draw("same p")
#
#legend2 = TLegend(0.1289398,0.6281513,0.5100287,0.8802521)
##legend.SetHeader("#tau_{h}-#tau_{h} channels","C")
#legend2.SetFillStyle(1001)
##legend1.AddEntry(hist_bbtt_higgspt_eff[0],"standard_VVLoose","ep")
#legend2.AddEntry(hist_bbtt_higgspt_eff_b[0],"boosted_VVLoose","ep")
##legend1.AddEntry(hist_bbtt_higgspt_eff[1],"standard_VLoose","ep")
#legend2.AddEntry(hist_bbtt_higgspt_eff_b[1],"boosted_Loose","ep")
##legend.AddEntry(hist_bbtt_higgspt_eff[2],"standard_Loose","ep")
#legend2.AddEntry(hist_bbtt_higgspt_eff_b[2],"boosted_Medium","ep")
##legend1.AddEntry(hist_bbtt_higgspt_eff[3],"standard_Medium","ep")
#legend2.AddEntry(hist_bbtt_higgspt_eff_b[3],"boosted_Tight","ep")
##legend1.AddEntry(hist_bbtt_higgspt_eff[4],"standard_Tight","ep")
##legend2.AddEntry(hist_bbtt_higgspt_eff_b[4],"boosted_Tight","ep")
##legend1.AddEntry(hist_bbtt_higgspt_eff[5],"standard_VTight","ep")
##legend2.AddEntry(hist_bbtt_higgspt_eff_b[5],"boosted_VTight","ep")
##legend1.AddEntry(hist_bbtt_higgspt_eff[6],"standard_VVTight","ep")
##legend2.AddEntry(hist_bbtt_higgspt_eff_b[6],"boosted_VVTight","ep")
#
#legend2.Draw("same")
#cmsLatex.SetTextSize(0.04)
#cmsLatex.SetNDC(True)
#cmsLatex.SetTextFont(61)
#cmsLatex.SetTextAlign(11)
#cmsLatex.DrawLatex(0.1,0.92,"CMS")
#cmsLatex.SetTextFont(52)
#cmsLatex.DrawLatex(0.1+0.07,0.92,"Preliminary")

##tt drawing
can2 = TCanvas("canvas2", "efficiency")
can2.SetGrid()
hist_bbtt_higgspt_eff[0].Draw("ap")
hist_bbtt_higgspt_eff_b[0].Draw("same p")
legend2 = TLegend(0.1289398,0.1,0.5100287,0.3)
#legend.SetHeader("#tau_{h}-#tau_{h} channels","C")

legend2.SetFillStyle(1001)
legend2.AddEntry(hist_bbtt_higgspt_eff_ttchannel[0],"standard_VVLoose","ep")
#legend.AddEntry(hist_bbtt_higgspt_eff_b[0],"boosted_VVLoose","ep")
legend2.AddEntry(hist_bbtt_higgspt_eff_b_ttchannel[0],"boosted_VVLoose","ep")
legend2.Draw()

cmsLatex.SetTextFont(61)
cmsLatex.SetTextAlign(11)
cmsLatex.DrawLatex(0.1,0.92,"CMS")
cmsLatex.SetTextFont(52)
cmsLatex.DrawLatex(0.1+0.07,0.92,"Preliminary")

channelLatex = TLatex()
channelLatex.SetTextSize(0.06)
channelLatex.SetNDC(True)
channelLatex.DrawLatex(0.8,0.8,"#tau_{h}#tau_{h}")

can2.SaveAs("ttchanneleff.png")
can2.SaveAs("ttchanneleff.pdf")


raw_input("Press Enter to continue...")
