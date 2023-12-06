#ifndef DISCASTERINPUTSTREAM_H
#define DISCASTERINPUTSTREAM_H


namespace Discaster {


  /**
   * Abstract base class representing an input stream.
   */
  class InputStream {
  public:
    InputStream();
    virtual ~InputStream();
    
    /**
     * Advances the stream and returns the next character.
     */
    virtual char get() =0;
    
    /**
     * Returns the next character, but does not advance the stream.
     */
    virtual char peek() =0;
    
    /**
     * Puts back the last read character from the stream with get().
     * Do not call more than once without first calling get() again.
     */
    virtual void unget() =0;
    
    /**
     * Returns true if input remains in the stream and false otherwise.
     */
    virtual bool eof() const =0;
    
    /**
     * Returns the current line number in the input stream.
     */
    virtual int lineNum() const =0;
    
  protected:
    
  };


}


#endif
