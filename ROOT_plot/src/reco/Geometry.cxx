#include "MuonReco/Geometry.h"

namespace MuonReco {
  
  Int_t Geometry::MAX_TDC          = 24;
  Int_t Geometry::MAX_TDC_CHANNEL  = 24;
  Int_t Geometry::MAX_TUBE_LAYER   =  8;
  Int_t Geometry::MAX_TUBE_COLUMN  = 65;
  Int_t Geometry::MAX_TDC_LAYER    =  4;
  Int_t Geometry::MAX_TDC_COLUMN   =  6;

  double Geometry::ML_distance     = 224.231;
  double Geometry::tube_length     = 1.1;

  double Geometry::layer_distance  = 13.0769836;
  double Geometry::column_distance = 15.1;
  double Geometry::radius          = 7.5;
  double Geometry::min_drift_dist  = 0.0;
  double Geometry::max_drift_dist  = 7.1;

  Geometry::Geometry() {
    runN         = 0;
    TRIGGER_CH   = 0;
    TRIGGER_MEZZ = 0;
    txtconfig_mode= kFALSE;

    isActiveTDC.resize(MAX_TDC);
    for (int i = 0; i < MAX_TDC; i++) {
      isActiveTDC[i] = 0;
    }

  }

  Geometry::Geometry(ConfigParser& cp) : Geometry() {
    Configure(cp.items("Geometry"));
  } // end method: Geometry initialization


  Geometry::~Geometry() {
    
  }

  void Geometry::Draw(int eventN) {
    axes.at(0)->SetTitle(";x [mm];z [mm]");
    axes.at(0)->Draw("AP");
    for (auto tube : drawable) {
      tube->Draw();
    }

  } // end method: Geometry :: Draw 


  void Geometry::Draw(int eventN, bool view) {
    axes.push_back((TGraph *)(axes.at(0)->DrawClone("AL")));
    if (view) {
      axes.at(0)->SetTitle(";y [mm];z [mm]");
      axes.at(0)->Draw("AP");
      for (auto tube : drawable) {
        int ML = (int)(tube->GetY1() / ML_distance);
        if (Geometry::orientation().at(ML) == 1) {
          tube->Draw();
        }
      }
    }
    else {
      axes.at(1)->SetTitle(";x [mm];z [mm]");
      axes.at(1)->Draw("AP");
      for (auto tube : drawable) {
        int ML = (int)(tube->GetY1() / ML_distance);
        if (Geometry::orientation().at(ML) == 0) {
          tube->Draw();
        }
      }
    }
  }

  void Geometry::Draw(int eventN, double xmin, double ymin, double xmax, double ymax, TString additionalText/*=""*/) {

    double xlim[2];
    double ylim[2];
    
    xlim[0] = xmin;
    xlim[1] = xmax;
    ylim[0] = ymin;
    ylim[1] = ymax;

    if (axes.size() == 1)
      axes.push_back(new TGraph(2,xlim,ylim));
    else
      axes[1] = new TGraph(2,xlim,ylim);
    
    axes.at(1)->SetMarkerColor(kWhite);
    axes.at(1)->SetTitle(";x [mm];z [mm]");
    axes.at(1)->Draw("AP");

    for (auto tube : drawable) {
      if (tube->GetX1() > xmin && tube->GetX1() < xmax && tube->GetY1() > ymin && tube->GetY1() < ymax) 
  tube->Draw();
    }
  }

  void Geometry::Draw(const char* title) {
    axes.at(0)->SetTitle(title);
    axes.at(0)->Draw("AP");
    for (auto tube : drawable) tube->Draw();
  }

  void Geometry::DrawTDCLabel() {
    for (auto t    : text) {
      t->Draw();
    }
  }

  void Geometry::ResetText() {
    text.clear();
    TString str;
    double hitX, hitY;
    int hitL, hitC;
    int testChan = 0;
    if (flipTDCs) {
      if (!chamberType.CompareTo("C")) testChan = 23;
      else if (!chamberType.CompareTo("A")) testChan = 22;      
    }
    else  {
      if (!chamberType.CompareTo("C")) testChan = 0;
      else if (!chamberType.CompareTo("A")) testChan = 1;
    }

    for (unsigned int tdc = 0; tdc != Geometry::MAX_TDC; tdc++) {
      if (IsActiveTDC(tdc) && tdc != TRIGGER_MEZZ) {
  GetHitLayerColumn(tdc, testChan, &hitL, &hitC);
  GetHitXY(hitL, hitC, &hitX, &hitY);
  str.Form("TDC %d", tdc);
  text.push_back(new TPaveText(hitX-9*radius, (hitY+3.5*layer_distance) + (Geometry::radius*0.03), hitX-3*radius, (hitY-0.5*layer_distance+ML_distance) - (Geometry::radius*0.03)));
  text.back()->AddText(str.Data());
      }
    }
    for (int i = 0; i < ML_orientation.size(); i++) {
      if (ML_orientation.at(i) == 1) {
        text.push_back(new TPaveText(column_distance * (MAX_TUBE_COLUMN+0.5), i * ML_distance, column_distance * (MAX_TUBE_COLUMN + 1), (i * ML_distance) + (layer_distance * 4)));
        text.back()->AddText("Perp");
        text.back()->SetTextAngle(-90);
        text.back()->SetTextAlign(32);
      }
    }
  }
  
  void Geometry::GetHitLayerColumn(unsigned int tdc_id, unsigned int channel_id, int *hit_layer, int *hit_column) const {
    if (txtconfig_mode ==true){
      if (TDC_ML[tdc_id]==0){
        *hit_column = (TDC_COL[tdc_id] + hit_column_map_ml0[channel_id])-1;
        *hit_layer  = hit_layer_map_ml0[channel_id]-1;
      }
      else{
        *hit_column = TDC_COL[tdc_id] + hit_column_map_ml1[channel_id]-1;
        *hit_layer  = hit_layer_map_ml1[channel_id]-1;
      }
    }
    else{
      if (tdcColByTubeNo) 
        *hit_column = TDC_COL[tdc_id] + hit_column_map[channel_id];
      else
        *hit_column = 6*TDC_COL[tdc_id] + hit_column_map[channel_id];

      *hit_layer  = 4*TDC_ML[tdc_id]  + hit_layer_map[channel_id];
    }
  }

  void Geometry::GetHitXY(int hitL, int hitC, double *hitX, double *hitY) {
    // get hit layer and column
    if (hitL < 0 || hitC < 0 ||hitL>8) {
      *hitX = -1;
      *hitY = -1;
      return;
    }
    else if (txtconfig_mode ==true){
      for (int i=0;i<txt_layer_column.size();i++){
        if (txt_layer_column[i][0] == hitL+1){
          if (txt_layer_column[i][1] ==hitC+1 ){
            *hitX = txt_x_y[i][0];
            *hitY = txt_x_y[i][1];
          }
        }
      }
    }
    else{
      if (!chamberType.CompareTo("C")) {
        *hitX = Geometry::radius + hitC * column_distance + ((hitL + 1) % 2) * column_distance / 2.0;
      }
      else if (!chamberType.CompareTo("A")) {
        *hitX = Geometry::radius + hitC * column_distance + (hitL % 2) * column_distance / 2.0;
      }
      if (MAX_TUBE_LAYER == layerOffset.size() && layerOffset.size() == layerSlope.size()) {
        *hitY = layerOffset.at(hitL) + layerSlope.at(hitL)*hitC;
      }

      *hitY = Geometry::radius + hitL * layer_distance  + (ML_distance-4*layer_distance) * MultiLayer(hitL);
    }
  }    

  int Geometry::MultiLayer(Cluster c) const {
    return c.Hits().at(0).Layer()/MAX_TDC_LAYER;
  }

  int Geometry::MultiLayer(int layer) {
    return layer/MAX_TDC_LAYER;
  }
  

  Bool_t Geometry::AreAdjacent(double x1, double y1, double x2, double y2) {
    return TMath::Sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)) < 1.2*column_distance;
  }
  
  Bool_t Geometry::AreAdjacent(Cluster c1, Cluster c2) {
    for (Hit h1 : c1.Hits()) {
      for (Hit h2 : c2.Hits()) {
  if (AreAdjacent(h1.X(), h1.Y(), h2.X(), h2.Y()))
    return kTRUE;   
      }
    }
    return kFALSE;
  }

  Bool_t Geometry::IsActiveTDC(unsigned int tdc) const {
    return isActiveTDC[tdc];
  }

  Bool_t Geometry::IsActiveTDCChannel(unsigned int tdc, unsigned int ch) const {
    if (tdc == TRIGGER_MEZZ)
      return 0;//ch == TRIGGER_CH;
    else {
      int hitL, hitC;
      GetHitLayerColumn(tdc, ch, &hitL, &hitC);
      if (hitC < 0 || hitC >= MAX_TUBE_COLUMN) {
        std::cout<<"Get wrong column "<<hitC<<std::endl;
        return 0;
      }
      if (hitL < 0 || hitL >= MAX_TUBE_LAYER) {
        std::cout<<"Get wrong layer "<<hitL<<std::endl;
        return 0;
      }
      return isActiveTDC[tdc];
    }
  }

  Bool_t Geometry::IsActiveLayerColumn(int layer, int column) const {
    int _tdc_layer, _tdc_column;
    Bool_t found = kFALSE;
    unsigned int iTDC;
    unsigned int iCh;
    for (iTDC = 0; iTDC < Geometry::MAX_TDC; iTDC++) {
      for (iCh = 0; iCh < Geometry::MAX_TDC_CHANNEL; iCh++) {
  GetHitLayerColumn(iTDC, iCh, &_tdc_layer, &_tdc_column);
  if (_tdc_layer == layer && _tdc_column == column) {
    found = kTRUE;
    break;
  }
      }
      if (found) break;
    }
    return IsActiveTDCChannel(iTDC, iCh);
  }

  int  Geometry::GetRunN() const {
    return runN;
  }

  void Geometry::SetRunN(int runNum) {
    runN = runNum;
  }

  void Geometry::Configure(ParameterSet ps) {
    Geometry::MAX_TDC         = ps.getInt("MAX_TDC",         24, 0);
    Geometry::MAX_TDC_CHANNEL = ps.getInt("MAX_TDC_CHANNEL", 24, 0);
    Geometry::MAX_TUBE_COLUMN = ps.getInt("MAX_TUBE_COLUMN", 65, 0);
    Geometry::MAX_TUBE_LAYER  = ps.getInt("MAX_TUBE_LAYER",  8,  0);
    Geometry::MAX_TDC_COLUMN  = ps.getInt("MAX_TDC_COLUMN",  6,  0);
    Geometry::MAX_TDC_LAYER   = ps.getInt("MAX_TDC_LAYER",   4,  0);

    Geometry::ML_distance     = ps.getDouble("ML_distance", 224.231, 0);
    Geometry::tube_length     = ps.getDouble("TubeLength",  1.5,     0);
    Geometry::layer_distance  = ps.getDouble("layer_distance", 13.0769836, 0);
    Geometry::column_distance = ps.getDouble("column_distance", 15.1, 0);
    Geometry::radius          = ps.getDouble("radius", 7.5, 0);
    Geometry::min_drift_dist  = ps.getDouble("min_drift_distance", 0.0, 0);
    Geometry::max_drift_dist  = ps.getDouble("max_drift_distance", 7.1, 0);

    // get run number and trigger information
    
    TRIGGER_MEZZ   = ps.getInt("TriggerMezz");
    TRIGGER_CH     = ps.getInt("TriggerChannel");
    chamberType    = ps.getStr("ChamberType", "C");
    tdcColByTubeNo = ps.getInt("TdcColByTubeNo", 0, 0);
    flipTDCs       = ps.getInt("FlipTDCs", 0, 0);
    txtconfig_mode = ps.getBool("IsMDT",0,0);
    /*
    TString flip   = ps.getStr("FlipTDCs", "0");
    TDC_FLIP.resize(Geometry::MAX_TDC);
    if (flip.Contains(":")) {
      int TDC;
      std::vector<int> activeTDCs = ps.getIntVector("ActiveTDCs");
      std::vector<int> tdc_flip = ps.getIntVector("FlipTDCs");
      for (size_t i = 0; i<tdc_flip.size(); ++i) {
  TDC         = activeTDCs.at(i);
  TDC_FLIP[TDC] = tdc_flip.at(i);
      }
      flipTDCs = TDC_FLIP[0];
    }
    else {
      flipTDCs       = ps.getInt("FlipTDCs", 0, 0);
      for (size_t i = 0; i<TDC_FLIP.size(); ++i) {
  TDC_FLIP[i] = flipTDCs;
      }
    }
    */
    
    isActiveTDC.resize(Geometry::MAX_TDC);
    int is_phase2_data = ps.getInt("IS_PHASE2_DATA", 0, 0);
    if(is_phase2_data==0)isActiveTDC[TRIGGER_MEZZ] = 1;

    TDC_ML .resize(Geometry::MAX_TDC);
    TDC_COL.resize(Geometry::MAX_TDC);
    hit_layer_map.resize(Geometry::MAX_TDC_CHANNEL);
    hit_column_map.resize(Geometry::MAX_TDC_CHANNEL);


    std::vector<int> activeTDCs     = ps.getIntVector("ActiveTDCs");
    std::vector<int> TDCMultiLayer  = ps.getIntVector("TDCMultilayer");
    std::vector<int> TDCColumn      = ps.getIntVector("TDCColumn");
    int MAX_ML = MAX_TUBE_LAYER / MAX_TDC_LAYER;
    if (ps.hasKey("ML_orientation")) {
      ML_orientation = ps.getIntVector("ML_orientation");
    }
    else {
      for (int i = 0; i < MAX_ML; i++) {
        ML_orientation.push_back(0);
      }
    }
    int nTDC = activeTDCs.size();
    nTDC = (nTDC < TDCMultiLayer.size()) ? nTDC : TDCMultiLayer.size();
    nTDC = (nTDC < TDCColumn.size())     ? nTDC : TDCColumn.size();

    int TDC;
    for (int i = 0; i < nTDC; i++) {
      TDC              = activeTDCs.at(i);
      TDC_ML     [TDC] = TDCMultiLayer.at(i);
      TDC_COL    [TDC] = TDCColumn.at(i);
      isActiveTDC[TDC] = 1;
      
      }

    if (ps.hasKey("LayerOffset")) {
      layerOffset = ps.getDoubleVector("LayerOffset");
    }
    if (ps.hasKey("LayerSlope")) {
      layerSlope = ps.getDoubleVector("LayerSlope");
    }

    // std::cout<<"MAX TDC = "<<MAX_TDC<<std::endl;
    if(txtconfig_mode)
      ConfigureByGeo("../conf/BIS7A08.txt");
    ResetTubeLayout();
  }

    // Use an external look-up table mapping
  // For the mapping file, the first raw should be fields.
  // The sequence of the fields should be mz,mz_ch,y,x,layer,column 
  void Geometry::ConfigureByGeo(std::string geoconfigpath) {

    hit_layer_map_ml0.resize(Geometry::MAX_TDC_CHANNEL);
    hit_column_map_ml0.resize(Geometry::MAX_TDC_CHANNEL);
    hit_layer_map_ml1.resize(Geometry::MAX_TDC_CHANNEL);
    hit_column_map_ml1.resize(Geometry::MAX_TDC_CHANNEL);
    std::ifstream file(geoconfigpath);
    if (!file.is_open()) {
        std::cout << "Error opening file!" << std::endl;
    }
    else{
      std::cout << "Geo config file opened." << std::endl;
      std::string line;
      std::getline(file, line);
      while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;

        // Vector to store data from current line
        std::vector<double> rowData;

        // Read data from each column
        while (std::getline(iss, token, '\t')) { // Assuming columns are separated by tabs
            rowData.push_back(atof(token.c_str()));
        }

        int txt_tdc_id = int(rowData[0]);
        int txt_tdc_channel = int(rowData[1])%100;
        int txt_layer = int(rowData[4]+(rowData[6]-1)*4);
        int txt_column = int(rowData[5]);
        double txt_x = rowData[3];
        double txt_y = rowData[2];

        std::vector<int> vect_txt_tdc_id_tdc_channel{txt_tdc_id,txt_tdc_channel};
        std::vector<int> vect_txt_layer_column{txt_layer,txt_column};

        //tdc channel mapping can be different for ml0 and ml1
        if (int(rowData[6]) == 1 ){//ml0 channel mapping mirror the x axis
          // if (txt_layer%2 == 1)txt_x-= column_distance / 2.0;
          // if (txt_layer%2 == 0)txt_x+= column_distance / 2.0;
          if (int(rowData[0])==0){
            hit_column_map_ml0[txt_tdc_channel] = txt_column;
            hit_layer_map_ml0[txt_tdc_channel] = txt_layer;
          }
        }
        if (int(rowData[6]) == 2 ){//ml1 channel mapping mirror the x axis
          // if (txt_layer%2 == 1)txt_x+= column_distance / 2.0;
          // if (txt_layer%2 == 0)txt_x-= column_distance / 2.0;
          if (int(rowData[0])==1){
            hit_column_map_ml1[txt_tdc_channel] = txt_column;
            hit_layer_map_ml1[txt_tdc_channel] = txt_layer;
          }
        }
        std::vector<double> vect_x_y{txt_x, txt_y};
        txt_tdc_id_tdc_channel.push_back(vect_txt_tdc_id_tdc_channel);
        txt_layer_column.push_back(vect_txt_layer_column);
        txt_x_y.push_back(vect_x_y);
        txt_ml.push_back(int(rowData[6]));
      }
    }
    file.close();
  }

  bool Geometry::IsPerpendicular(int layer) {
    int ml = MultiLayer(layer);
    bool is_perp = (bool)Geometry::ML_orientation.at(ml);
    return is_perp;
  }

  std::vector<int> Geometry::orientation() {
    return ML_orientation;
  }

  void Geometry::ResetTubeLayout() {
    if(!txtconfig_mode){
      for (int i = 0; i < Geometry::MAX_TDC_CHANNEL; i++) {
        if (!chamberType.CompareTo("C")) {
          if (flipTDCs) {
            hit_layer_map[i] = 3-(i%4);
                  hit_column_map[i] = i/4;
          }
          else {
            hit_layer_map[i] = i%4;
            hit_column_map[i] = 5 - i/4;
          }
        }
        else if (!chamberType.CompareTo("A")) {
          if (flipTDCs) {
            hit_layer_map[i] = (i+2)%4;
                  hit_column_map[i] = i/4;
          }
          else {
            hit_column_map[i] = 5 - i/4;
            hit_layer_map[i] = (i%2) ? (i%4)-1 : (i%4)+1;
          }
        }
      }
    }

    double hitX, hitY;
    GetHitXY(MAX_TUBE_LAYER-1, MAX_TUBE_COLUMN-1, &hitX, &hitY);
    track_corner_x[0] = 0;
    track_corner_x[1] = hitX+radius*1.1;
    track_corner_y[0] = 0;
    track_corner_y[1] = hitY+radius*1.1;
    axes.clear();
    axes.push_back(new TGraph(2, track_corner_x, track_corner_y));

    drawable.clear();
    // create the background of the event display: the outlines of the tubes
    for (Int_t layer_id = 0; layer_id != Geometry::MAX_TUBE_LAYER; layer_id++) {
      for (Int_t column_id = 0; column_id != Geometry::MAX_TUBE_COLUMN; column_id++) {
  GetHitXY(layer_id, column_id, &center_x, &center_y);
        drawable.push_back(new TEllipse(center_x, center_y, radius, radius));

        if ((column_id / 6) % 2 == 0) {
          drawable.at(layer_id*Geometry::MAX_TUBE_COLUMN + column_id)->SetFillColor(kGray);
        }
      }
    }

    axes.at(0)->SetTitle("event");
    axes.at(0)->SetMarkerStyle(0);
    ResetText();
  }

  double Geometry::getMeanYPosition() {
    int ML = MultiLayer(MAX_TUBE_LAYER);
    return ((ML - 1) * ML_distance + (MAX_TDC_LAYER - 1) * layer_distance + 2.0 * radius)/2.0;
  }

}
