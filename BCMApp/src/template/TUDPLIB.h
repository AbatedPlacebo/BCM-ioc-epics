template<typename DEV>
class TUDPLIB: public UDPLIB
{
	public:
	TUDPLIB():UDPLIB() {}
	~TUDPLIB() {}

	typedef DEV INFO;

	int write_reg(unsigned int regn, unsigned int param);
	int read_reg(unsigned int regn, unsigned int *param);
	int start();
	int stop();
};

//#include"chk_dt.h"
#include"chk.h"
extern int debug_level;

#define CUNET_PRINT(_debug_level, str, buf, size) \
	do{ \
		D(_debug_level,(" ")); \
		UDPLIB::cunet_print(_debug_level, str,buf,size); \
	}while(0)


template<typename DEV>
int TUDPLIB<DEV>::write_reg(unsigned int regn, unsigned int param)
{
    int err = -1;
	uint8_t ack[4];
	int cnt;
	int rep = 1;
REP:
	D(3,("write_reg %i %i(%04x)\n", regn, param, param));
	CHK(err = send_com(DEV::CMD_WRREG, regn, param));
	for(cnt = 1; cnt > 0; --cnt) {
		CHK(err = recv_to(ack, sizeof(ack), 10/*, 0*/));
		if(err == 0 && rep > 0) {
			D(2,("repeat %i TUDPLIB<>::write_reg\n", rep));
			--rep;
			goto REP;
		}
		if(err == 2 && ack[0] == 0x11) {
			++cnt;
			continue;
		}
		CHKTRUEMESG(err == sizeof(ack),("err=%i\n", err));
		CHKTRUE(ack[0] == 0x10);
		CHKTRUE(ack[1] == DEV::CMD_WRREG);
		CHKTRUE(ack[2] == regn);
		CHKTRUE(ack[3] == 0x0f || ack[3] == 0x20);
	}
	//if(regn < BPMREG_SIZE)
	//	regs[regn] = param;
    return err;
CHK_ERR:
	if(err > 0)
		D(2,("err=%i ack=%02x%02x%02x%02x\n", err, ack[0], ack[1], ack[2], ack[3]));
    return -1;
}

template<typename DEV>
int TUDPLIB<DEV>::read_reg(unsigned int regn, unsigned int *param)
{
	int err = -1;
	uint8_t ack[4];
	int cnt;
	int rep = 1;
REP:
	D(3,("read_reg %i\n", regn));
    CHK(err = send_com(DEV::CMD_RDREG, regn, 0));
	for(cnt = 2; cnt > 0; --cnt) {
		CHK(err = recv_to(ack, sizeof(ack), 10/*, 0*/));
		if(err == 0 && rep > 0) {
			D(2,("repeat %i TUDPLIB<>::read_reg %s\n", rep, __FUNCTION__));
			--rep;
			goto REP;
		}
		if(err == 2 && ack[0] == 0x11) {
			++cnt;
			m_conf++;
			continue;
		}
		CHKTRUE(err == sizeof(ack));
		CHKTRUE(ack[0] == 0xF4 || ack[0] == 0x10);
		if(ack[0] == 0x10) {
			int pack = 1;
			WARNTRUE(ack[0] == 0x10                     || (pack = 0));
			WARNTRUE(ack[1] == DEV::CMD_RDREG           || (pack = 0));
			WARNTRUE(ack[2] == regn                     || (pack = 0));
			WARNTRUE((ack[3] == 0x0f || ack[3] == 0x20) || (pack = 0));
			if(pack == 0)
				D(2,("err=%i ack=%02x%02x%02x%02x\n", err, ack[0], ack[1], ack[2], ack[3]));
		}
		else if(ack[0] == 0xf4) {
			int pack = 1;
			WARNTRUE(ack[0] == 0xF4                     || (pack = 0));
			WARNTRUE(ack[1] == regn                     || (pack = 0));
			if(pack == 0)
				D(2,("err=%i ack=%02x%02x%02x%02x\n", err, ack[0], ack[1], ack[2], ack[3]));
			*param = (((unsigned int) ack[2]) << 8) | ack[3];
			D(3,("val %i(%04x)\n", *param, *param));
		}
	}
	return err;

  CHK_ERR:
	if(err > 0)
		CUNET_PRINT(2, "ack", ack, err);
	return -1;
}


template<typename DEV>
int TUDPLIB<DEV>::start()
{
	int err = -1;
	uint8_t ack[4];
	conf();
	CHK(err = send_com(DEV::CMD_START, 0, 0));
	CHK(err = recv_to(ack, sizeof(ack), 10/*, 0*/));
	CHKTRUE(err == sizeof(ack));
	CHKTRUE(ack[0] == 0x10);
	CHKTRUE(ack[1] == DEV::CMD_START);
	CHKTRUE(ack[2] == 0);
	CHKTRUE(ack[3] == 0x0f || ack[3] == 0x20);
	return err;
CHK_ERR:
	if(err > 0)
		CUNET_PRINT(2, "ack", ack, err);
	return -1;
}


template<typename DEV>
int TUDPLIB<DEV>::stop()
{
	int err = -1;
	uint8_t ack[4];
	int cnt;
	int rep = 1;
REP:
	CHK(err = send_com(DEV::CMD_STOP, 0, 0));
	for(cnt = 1; cnt > 0; --cnt) {
		CHK(err = recv_to(ack, sizeof(ack), 10/*, 0*/));
		if(err == 0 && rep > 0) {
			D(2,("repeat %i %s\n", rep, __FUNCTION__));
			--rep;
			goto REP;
		}
		if(err == 2 && ack[0] == 0x11) {
			++cnt;
			continue;
		}
		CHKTRUE(err == sizeof(ack));
		CHKTRUE(ack[0] == 0x10);
		CHKTRUE(ack[1] == DEV::CMD_STOP);
		CHKTRUE(ack[2] == 0);
		CHKTRUE(ack[3] == 0x0f || ack[3] == 0x20);
	}
	conf();
	return err;
CHK_ERR:
	conf();
	if(err > 0)
		CUNET_PRINT(2, "ack", ack, err);
	return -1;
}


/***********************************************************/


//using CAL_t = TUDPLIB<DEVCAL>;
//using BPM_t = TUDPLIB<DEVBPM>;

#endif
