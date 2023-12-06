#ifndef DISCASTERCONFIG_H
#define DISCASTERCONFIG_H


namespace Discaster {


  class Config {
  public:
    Config();
    
    bool debugOutput() const;
    void setDebugOutput(bool debugOutput__);
    
    bool warningsOn() const;
    void setWarningsOn(bool warningsOn__);
    
  protected:
    bool debugOutput_;
    bool warningsOn_;
  };
  
  extern Config config;


}


#endif
