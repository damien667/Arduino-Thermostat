#if !defined(__FTDIASYNC_H__)
#define __FTDIASYNC_H__

class FTDIAsync : 
public FTDIAsyncOper
{
public:
  virtual uint8_t OnInit(FTDI *pftdi);
};

uint8_t FTDIAsync::OnInit(FTDI *pftdi)
{
  uint8_t rcode = 0;

  rcode = pftdi->SetBaudRate(9600);

  if (rcode)
  {
    Serial.println("Error with SsetBaudRate!");
    ErrorMessage<uint8_t>(PSTR("SetBaudRate"), rcode);
    return rcode;
  }
  rcode = pftdi->SetFlowControl(FTDI_SIO_DISABLE_FLOW_CTRL);

  if (rcode) {
    Serial.println("Error with SetFlowControl!");
    ErrorMessage<uint8_t>(PSTR("SetFlowControl"), rcode);
  }

  rcode = pftdi->SetData(FTDI_SIO_SET_DATA_PARITY_NONE | FTDI_SIO_SET_DATA_STOP_BITS_1);

  if (rcode) {
    Serial.println("Error with SetData!");
    ErrorMessage<uint8_t>(PSTR("SetData"), rcode);
  }

  return rcode;
}

#endif // __FTDIASYNC_H__
