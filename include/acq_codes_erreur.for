       PARAMETER  ACQ_OK = '08018001'X
C     <you know what? I'm HAPPY!>
       PARAMETER  ACQ_ON = '08018009'X
C     <your acquisition is running>
       PARAMETER  ACQ_OFF = '08018011'X
C     <your acquisition is halted>
       PARAMETER  ACQ_ADDTAPEOK = '08018019'X
C     <successfull adding operation of a storage device >
       PARAMETER  ACQ_REMTAPEOK = '08018021'X
C     <successfull removing operation of a storage device >
       PARAMETER  ACQ_ENABTAPEOK = '08018029'X
C     <successfull enabling operation of storage device >
       PARAMETER  ACQ_DISABTAPEOK = '08018031'X
C     <successfull disabling operation of storage device >
       PARAMETER  ACQ_MOUNTTAPEOK = '08018039'X
C     < successfull mount operation >
       PARAMETER  ACQ_INIACQEXEC = '08018041'X
C     <successfull execution of initialization of the VME crate>
       PARAMETER  ACQ_TIMOUT = '0801804B'X
C     <going out of SEMWAIT through time out condition>
       PARAMETER  ACQ_MBXNOMSG = '08018053'X
C     <there is no message in that mailbox>
       PARAMETER  ACQ_PURGEDPID = '0801805B'X
C     <a communication session was purged...>
       PARAMETER  ACQ_DBGMOD = '08018063'X
C     <starting debug mode for acquisition>
       PARAMETER  ACQ_NODBGMOD = '0801806B'X
C     <ending debug mode for acquisition>
       PARAMETER  ACQ_CTRLMOD = '08018073'X
C     <starting control mode for acquisition>
       PARAMETER  ACQ_FULLCTRLMOD = '0801807B'X
C     <starting full control mode for acquisition>
       PARAMETER  ACQ_STNDCTRLMOD = '08018083'X
C     <starting standard control mode for acquisition (no user call)>
       PARAMETER  ACQ_USERCTRLMOD = '0801808B'X
C     <starting user control mode for acquisition>
       PARAMETER  ACQ_NOCTRLMOD = '08018093'X
C     <no more control is performed on acquisition>
       PARAMETER  ACQ_STOREMOD = '0801809B'X
C     <starting store mode for acquisition>
       PARAMETER  ACQ_NOSTOREMOD = '080180A3'X
C     <acquisition goes on without storage of datas>
       PARAMETER  ACQ_SIMMOD = '080180AB'X
C     <starting simulation mode for acquisition>
       PARAMETER  ACQ_NOSIMMOD = '080180B3'X
C     <ending simulation mode for acquisition>
       PARAMETER  ACQ_CONNECTION = '080180BB'X
C     <a new communication session starts now...>
       PARAMETER  ACQ_DISCONNECT = '080180C3'X
C     <a communication session stops!>
       PARAMETER  ACQ_ASKFORABORT = '080180CB'X
C     <received a demand to abort the application>
       PARAMETER  ACQ_TAPEISFULL = '080180D3'X
C     <tape was full,and dismounted, load another if necessary and mount it>
       PARAMETER  ACQ_FREECTRLBUF = '080180DB'X
C     <a buffer was freed up without control>
       PARAMETER  ACQ_ENDOFBUFFER = '080180E3'X
C     <the buffer doesn't contain any other event>
       PARAMETER  ACQ_NOACQUIS = '080180EB'X
C     <the GANIL acquisition is not started>
       PARAMETER  ACQ_VISUOFFLINE = '080180F3'X
C     <the visualisation runs off line>
       PARAMETER  ACQ_VISUNOCONNECT = '080180FB'X
C     <the visualisation is not actually connected to host>
       PARAMETER  ACQ_FREEBUFONSTOP = '08018103'X
C     <when acquisition stopped, some buffers were freed up>
       PARAMETER  ACQ_FREEBUFONCRASH = '0801810B'X
C     <on crash of control process, some buffers were freed up>
       PARAMETER  ACQ_ENDOFFILE = '08018113'X
C     <attempt to read past end of file>
       PARAMETER  ACQ_ENDOFTAPE = '0801811B'X
C     <attempt to read past end of tape>
       PARAMETER  ACQ_DUMPALLMOD = '08018123'X
C     <starting dump all buffers mode>
       PARAMETER  ACQ_DUMPNONEMOD = '0801812B'X
C     <starting nodump buffers mode>
       PARAMETER  ACQ_DUMPSTOREMOD = '08018133'X
C     <starting dump store buffers mode>
       PARAMETER  ACQ_DUMPCTRLMOD = '0801813B'X
C     <starting dump control buffers mode>
       PARAMETER  ACQ_NEWCTRLTASK = '08018143'X
C     <starting with a new version of user control program>
       PARAMETER  ACQ_NEWREADTASK = '0801814B'X
C     <reload the read VME task (ACQ_READ_DATA)>
       PARAMETER  ACQ_NEWSFROMVME = '08018153'X
C     <received from VME crate this informational message>
       PARAMETER  ACQ_EXIT = '0801815B'X
C     <ask to exit a command session>
       PARAMETER  ACQ_REPLAYMOD = '08018163'X
C     <starting replay mode for acquisition>
       PARAMETER  ACQ_NOREPLAYMOD = '0801816B'X
C     <starting normal mode for acquisition>
       PARAMETER  ACQ_NOMOREVINBUF = '08018173'X
C     <no more event in that buffer>
       PARAMETER  ACQ_RETPROCCTRL = '0801817B'X
C     <something has been sent back to ACQUISITION process>
       PARAMETER  ACQ_READSTRT = '08018183'X
C     <starting ACQ_READ_DATA process>
       PARAMETER  ACQ_CTRLSTRT = '0801818B'X
C     <starting ACQ_CTRL_DATA process>
       PARAMETER  ACQ_STORESTRT = '08018193'X
C     <starting ACQ_STORE_DATA process>
       PARAMETER  ACQ_NOFILTDEF = '0801819B'X
C     <no filter defined>
       PARAMETER  ACQ_OPNOTCOMP = '080181A3'X
C     <operation is not completed, wait a moment...>
       PARAMETER  ACQ_VMEINIFILEXEC = '080181AB'X
C     <auto execution of VME initialization file>
       PARAMETER  ACQ_VMESTAFILEXEC = '080181B3'X
C     <auto execution of VME start of run file>
       PARAMETER  ACQ_VMEFIRFILEXEC = '080181BB'X
C     <auto execution of VME FIRST IN RUN file>
       PARAMETER  ACQ_VMEENDFILEXEC = '080181C3'X
C     <auto execution of VME end of run file>
       PARAMETER  ACQ_ARGALL = '080181CB'X
C     <'all' character argument specified as command parameter>
       PARAMETER  ACQ_NEWSTORETASK = '080181D3'X
C     <reload the storage task (ACQ_STORE_DATA)>
       PARAMETER  ACQ_AUTENDRUN = '080181DB'X
C     <automatic end of run number >
       PARAMETER  ACQ_STWHAUTENAB = '080181E3'X
C     <stop acquisition (automatic mode is disabled, or no tape is available)>
       PARAMETER  ACQ_NOTIMPL = '08018960'X
C     <not yet implemented>
       PARAMETER  ACQ_GLTOOSHORT = '08018968'X
C     <global section contains less bytes than requested>
       PARAMETER  ACQ_NOCONNECT = '08018970'X
C     <got a message without connection code...>
       PARAMETER  ACQ_NOMORESESSION = '08018978'X
C     <session table is full>
       PARAMETER  ACQ_UNEXPMSG = '08018980'X
C     <the header of this message does not match with acquisition rules>
       PARAMETER  ACQ_UNEXPCODE = '08018988'X
C     <the action code of this message is unknown >
       PARAMETER  ACQ_NOPRIV = '08018990'X
C     <you don't have the required privilege for this operation!>
       PARAMETER  ACQ_PRIVALRATTR = '08018998'X
C     <the privilege you want was previously attributed>
       PARAMETER  ACQ_PRIVNOTALLOWED = '080189A0'X
C     <the privilege you want requires a password>
       PARAMETER  ACQ_ACTNOTAUT = '080189A8'X
C     <this action is not authorized in this context>
       PARAMETER  ACQ_TOOMUCHTAPE = '080189B0'X
C     <you already have maximal allowed number of storage devices>
       PARAMETER  ACQ_ALRALLOC = '080189B8'X
C     <this storage device is already yours>
       PARAMETER  ACQ_NOTALLOC = '080189C0'X
C     <this storage device is not actually yours>
       PARAMETER  ACQ_NOMORETAPE = '080189C8'X
C     <you don't have any storage device for your acquisition>
       PARAMETER  ACQ_NOTMOUNT = '080189D0'X
C     <the tape on which you want to work is not mounted>
       PARAMETER  ACQ_ALREADYMOUNT = '080189D8'X
C     <the tape you want to mount is already mounted>
       PARAMETER  ACQ_ISNOTATAPE = '080189E0'X
C     <the name you enter doesn't represent a storage device>
       PARAMETER  ACQ_TAPEINUSE = '080189E8'X
C     <this storage device is actually used by your acquisition>
       PARAMETER  ACQ_TAPENODISP = '080189F0'X
C     <this storage device is not actually available>
       PARAMETER  ACQ_BADEVENTFORM = '080189F8'X
C     <the format of this event is not compatible with IN2P3 standard>
       PARAMETER  ACQ_SPECTABFULL = '08018A00'X
C     <you already got the maximal allowed count of spectra >
       PARAMETER  ACQ_PARTABFULL = '08018A08'X
C     <you already got the maximal allowed count of parameters >
       PARAMETER  ACQ_DUPPLNAME = '08018A10'X
C     <this name is already in this table>
       PARAMETER  ACQ_UNKNSPEC = '08018A18'X
C     <this spectrum was not defined in the table.Check name or number>
       PARAMETER  ACQ_SPFULLSPACE = '08018A20'X
C     <the whole space dedicated to spectra incrementation is used>
       PARAMETER  ACQ_TOOMANYSPEC = '08018A28'X
C     <too many spectra to start at the same time>
       PARAMETER  ACQ_SPALRINC = '08018A30'X
C     <some of these spectra are already incremented>
       PARAMETER  ACQ_SPWRNGALRINC = '08018A38'X
C     <some spectra to start were already being incremented>
       PARAMETER  ACQ_SPCLRGALRINC = '08018A40'X
C     <the spectra already incremented have been cleared>
       PARAMETER  ACQ_SPNOTALLSUSP = '08018A48'X
C     <some of the spectra you want to restart were not suspended>
       PARAMETER  ACQ_SPNOTALLINC = '08018A50'X
C     <some of the spectra you want to suspend are not started>
       PARAMETER  ACQ_SPNOTINTABLE = '08018A58'X
C     <the spectrum you want to replace is not in the dictionnary>
       PARAMETER  ACQ_INCRSPEC = '08018A60'X
C     <the spectrum you want to replace is actually incremented>
       PARAMETER  ACQ_PARNOTINTABLE = '08018A68'X
C     <the parameter you want to replace was not defined>
       PARAMETER  ACQ_USEDPARAM = '08018A70'X
C     <the parameter you want to replace is actually in use>
       PARAMETER  ACQ_NOSUCHPAR = '08018A78'X
C     <one parameter you want to use doesn't exist>
       PARAMETER  ACQ_NOSUCHCOEF = '08018A80'X
C     <the coefficient you want to change doesn't exist>
       PARAMETER  ACQ_COEFTABFULL = '08018A88'X
C     <you already got the maximal allowed count of user coefficients>
       PARAMETER  ACQ_ERALLOCVM = '08018A90'X
C     <virtual memory allocation failure >
       PARAMETER  ACQ_NOTALLSTOPPED = '08018A98'X
C     <all spectra cannot be stopped (remote connection ??)>
       PARAMETER  ACQ_ERCODERSIZE = '08018AA0'X
C     <more bits were coded for this parameter than in description>
       PARAMETER  ACQ_STORNONEXIST = '08018AA8'X
C     <the spectrum you want save doesn't exist actually>
       PARAMETER  ACQ_ERRLOGTAB = '08018AB0'X
C     <error detected in logical tables...>
       PARAMETER  ACQ_NOSUCHSPEC = '08018AB8'X
C     <this spectrum was not defined>
       PARAMETER  ACQ_NOINCRSPEC = '08018AC0'X
C     <this spectrum is not actually incremented>
       PARAMETER  ACQ_CHRUNWHILACT = '08018AC8'X
C     <cannot change run number while acquisition runs>
       PARAMETER  ACQ_CTRLUSRERR = '08018AD0'X
C     <this error was signaled in the user part of control process>
       PARAMETER  ACQ_UNKNCOEF = '08018AD8'X
C     <this coeff was not defined in the table.Check name or number>
       PARAMETER  ACQ_UNKNPARAM = '08018AE0'X
C     <this parameter was not defined in the table.Check name or number>
       PARAMETER  ACQ_LOSTCONNECTION = '08018AE8'X
C     <sorry,lost connection with DECnet partner>
       PARAMETER  ACQ_ACQALRRUN = '08018AF0'X
C     <Your acquisition is already running...>
       PARAMETER  ACQ_INVPASSW = '08018AF8'X
C     <Invalid password>
       PARAMETER  ACQ_DEVNOTMOUNT = '08018B00'X
C     <The device you want to access is not mounted>
       PARAMETER  ACQ_DEVWRITLOCK = '08018B08'X
C     <This device is write-locked>
       PARAMETER  ACQ_CTRLBUSY = '08018B10'X
C     <The control process is busy...Try again later!>
       PARAMETER  ACQ_NOCHGWHRUN = '08018B18'X
C     <cannot reload any process while acquisition is running>
       PARAMETER  ACQ_NOABTWHMOUN = '08018B20'X
C     <Please dismount tapes before abort aquisition process>
       PARAMETER  ACQ_ERVMESYNC = '08018B28'X
C     < Some error occurs during synchronization with remote VME crate>
       PARAMETER  ACQ_WARNFROMVME = '08018B30'X
C     <received from VME crate this warning message>
       PARAMETER  ACQ_NOINIWHRUN = '08018B38'X
C     <cannot init VME crate while acquisition is running>
       PARAMETER  ACQ_BUFALREADYGOT = '08018B40'X
C     <current control buffer was already read from your task>
       PARAMETER  ACQ_NOCTRLBUFDISP = '08018B48'X
C     <there is currently no control buffer because acq is stopped >
       PARAMETER  ACQ_ERRCTRLBUF = '08018B50'X
C     <an error occurs in control buffer copy (twice changed)>
       PARAMETER  ACQ_NOCURRCTRLBUF = '08018B58'X
C     <there is no available ctrl buffer, please try again>
       PARAMETER  ACQ_RAFRFULL = '08018B60'X
C     <auxilliary array for refreshment of spectrum is full>
       PARAMETER  ACQ_WRBUFCTRL = '08018B68'X
C     <will store a control buffer, please contact manager>
       PARAMETER  ACQ_TOOMANYINCR = '08018B70'X
C     <too many incremented spectra related to a parameter>
       PARAMETER  ACQ_CNTOU_INVBITMP = '08018B78'X
C     <invalid bitmap dimensions>
       PARAMETER  ACQ_CNTOU_OUTBITMP = '08018B80'X
C     <contour not within related bitmap>
       PARAMETER  ACQ_CNTOU_TOOTWIST = '08018B88'X
C     <too many intersections between contour and horizontal line>
       PARAMETER  ACQ_CNTOU_FUNNY = '08018B90'X
C     <something weird in the contour point list ; check it>
       PARAMETER  ACQ_TOOMANYPOINTS = '08018B98'X
C     <too many points in the current contour>
       PARAMETER  ACQ_INVPOINTNUMB = '08018BA0'X
C     <invalid points number>
       PARAMETER  ACQ_NOSUCHCNTOU = '08018BA8'X
C     <this contour was not defined>
       PARAMETER  ACQ_INVCONTSCAL = '08018BB0'X
C     <invalid contour scaling factor>
       PARAMETER  ACQ_INVCONTNAM = '08018BB8'X
C     <invalid contour name>
       PARAMETER  ACQ_CONTOUTABFULL = '08018BC0'X
C     <you already got the maximal allowed count of contours >
       PARAMETER  ACQ_UNKNCONTOUR = '08018BC8'X
C     <this contour was not defined in the table.Check name or number>
       PARAMETER  ACQ_FAMINOCNTOU = '08018BD0'X
C     <No contour belongs to this family. Check name>
       PARAMETER  ACQ_CNTOUNOTIMPL = '08018BD8'X
C     <type of contour not yet implemented>
       PARAMETER  ACQ_BITMAPTABFULL = '08018BE0'X
C     <you already got the maximal allowed count of bitmaps >
       PARAMETER  ACQ_NOBITMAP = '08018BE8'X
C     <No bitmap for this filter>
       PARAMETER  ACQ_INVFILTNAM = '08018BF0'X
C     <invalid filter name>
       PARAMETER  ACQ_UNKNFILTER = '08018BF8'X
C     <this filter was not defined in the table.Check name or number>
       PARAMETER  ACQ_FILTERTABFULL = '08018C00'X
C     <you already got the maximal allowed count of filters >
       PARAMETER  ACQ_INVFILTSYNTX = '08018C08'X
C     <invalid filter syntax>
       PARAMETER  ACQ_INVPARVAL = '08018C10'X
C     <invalid parameter value specified>
       PARAMETER  ACQ_DUPPLFILTER = '08018C18'X
C     <this filter is already in the table>
       PARAMETER  ACQ_TOOLONGFILTDEF = '08018C20'X
C     <too long filter def. string; split it into several filters>
       PARAMETER  ACQ_NOFILDELWHCOND = '08018C28'X
C     <Warning ! A spectrum is conditionned by this filter !>
       PARAMETER  ACQ_NOFILDELWHCOMP = '08018C30'X
C     <Warning ! This filter is used to define a compound filter !>
       PARAMETER  ACQ_INVFILESPEC = '08018C38'X
C     <invalid file specification>
       PARAMETER  ACQ_NOTAPACTWHRUN = '08018C40'X
C     <skip actions on tape are not allowed while running>
       PARAMETER  ACQ_VMEINIFILNOFND = '08018C48'X
C     <VME initialization file not found for auto execution>
       PARAMETER  ACQ_VMESTAFILNOFND = '08018C50'X
C     <VME start of run file not found for auto execution>
       PARAMETER  ACQ_VMEFIRFILNOFND = '08018C58'X
C     <VME FIRST IN RUN file not found for auto execution>
       PARAMETER  ACQ_VMEENDFILNOFND = '08018C60'X
C     <VME end of run file not found for auto execution>
       PARAMETER  ACQ_NONSTWHMOUN = '08018C68'X
C     <Please dismount tapes before reload storage process>
       PARAMETER  ACQ_NOMODCHGWHRUN = '08018C70'X
C     <buffer selection mode is not allowed while acquisition is running>
       PARAMETER  ACQ_ERRPARAM = '08018C82'X
C     <parameter error>
       PARAMETER  ACQ_VMAXTBIG = '08018C8A'X
C     <maximal value out of range>
       PARAMETER  ACQ_VMINTBIG = '08018C92'X
C     <minimal value out of range>
       PARAMETER  ACQ_VTOOBIG = '08018C9A'X
C     <value out of range>
       PARAMETER  ACQ_CNXREFUS = '08018CA2'X
C     <connection to ACQUISITION refused>
       PARAMETER  ACQ_TOOMUCHCHAR = '08018CAA'X
C     <too much characters in this definition >
       PARAMETER  ACQ_RAFBUFOVF = '08018CB2'X
C     <overflow of the refresh buffer>
       PARAMETER  ACQ_NOREPCTRL = '08018CBA'X
C     <the ACQ_CTRL_DATA process doesn't answer...sorry!>
       PARAMETER  ACQ_IOFORT = '08018CC2'X
C     <i/o error detected in fortran calls>
       PARAMETER  ACQ_INVARG = '08018CCA'X
C     <invalid or missing argument(s) in your command>
       PARAMETER  ACQ_ERRDURREAD = '08018CD2'X
C     <error occurs while reading a tape or disk file>
       PARAMETER  ACQ_ERRNUMSPEC = '08018CDA'X
C     <call to acq_inc_user... routines for unknown spectra>
       PARAMETER  ACQ_ERRTYPSPEC = '08018CE2'X
C     <call to acq_inc_user... routines for spectra of another type>
       PARAMETER  ACQ_ERRSIZSPEC = '08018CEA'X
C     <call to acq_inc_user... with an index larger than the spectra size>
       PARAMETER  ACQ_BADTAPEFORM = '08018CF2'X
C     <format of tape or disk file is not compatible with IN2P3 standard>
       PARAMETER  ACQ_ERRFROMVME = '08018CFA'X
C     <a fatal error message was issued from VME crate>
       PARAMETER  ACQ_VMECOMLEN = '08018D02'X
C     <Length of vme command line too big>
       PARAMETER  ACQ_TAPEISVERYFULL = '08018D0A'X
C     <problem of synchro with VME, and must close the file on tape>
       PARAMETER  ACQ_VMETCPNOMEM = '08018D12'X
C     < TCPVME: not enough memory >
       PARAMETER  ACQ_TCPREPTOOSMALL = '08018D1A'X
C     < TCPVME: answer buffer is too small>
       PARAMETER  ACQ_IOFAMFILE = '08018D22'X
C     <Input/Output error with the spectrum family file>
       PARAMETER  ACQ_ERRSETSYMBFAM = '08018D2A'X
C     <Cannot set the symbol for one spectra family>
       PARAMETER  ACQ_STOSPECIO = '08018D32'X
C     <i/o error in storage operation of spectra>
       PARAMETER  ACQ_STOCONTOURIO = '08018D3A'X
C     <i/o error in storage operation of contours>
       PARAMETER  ACQ_READCONTOURIO = '08018D42'X
C     <i/o error in reading operation of contours>
       PARAMETER  ACQ_ERRFILTFILEIO = '08018D4A'X
C     <error in filter file input/output>
       PARAMETER  ACQ_INVDATINBUF = '08018D52'X
C     <a buffer contains invalid datas, contact system manager>
       PARAMETER  ACQ_NODEVMOUNT = '08018D5A'X
C     <no storage operation is possible because no device is mounted>
       PARAMETER  ACQ_NODLINVALID = '08018FA4'X
C     <the ODL'number is invalid (not initialized or false)>
       PARAMETER  ACQ_READEXIT = '08018FAC'X
C     <the process ACQ_READ_DATA exits now>
       PARAMETER  ACQ_STOREEXIT = '08018FB4'X
C     <the process ACQ_STORE_DATA exits now>
       PARAMETER  ACQ_CTRLEXIT = '08018FBC'X
C     <the process ACQ_CTRL_DATA exits now>
       PARAMETER  ACQ_ACQUISEXIT = '08018FC4'X
C     <the process ACQUISITION exits now>
       PARAMETER  ACQ_PRCRASH = '08018FCC'X
C     <one of the acquisition process recently crashed>
       PARAMETER  ACQ_FATERRREC = '08018FD4'X
C     < did receive a fatal error code message >
       PARAMETER  ACQ_ERRFICCOEF = '08018FDC'X
C     <error detected in the initialization file of user coefficients>
       PARAMETER  ACQ_ERRFICSPECTRE = '08018FE4'X
C     <error detected in the initialization file of spectra>
       PARAMETER  ACQ_ERRFICPARBR = '08018FEC'X
C     <error detected in the initialization file of parameters>
       PARAMETER  ACQ_ERRFICPARCA = '08018FF4'X
C     <error detected in the initialization file of calculated parameters>
       PARAMETER  ACQ_ERRDICOSPEC = '08018FFC'X
C     <error detected in spectra's description tables>
       PARAMETER  ACQ_ERRDICOPAR = '08019004'X
C     <error detected in parameters' description tables>
       PARAMETER  ACQ_ERRDICOCNTOU = '0801900C'X
C     <error detected in contours' description tables>
       PARAMETER  ACQ_ERRDICOFILT = '08019014'X
C     <error detected in filters' description tables>
       PARAMETER  ACQ_ERREVTCALC = '0801901C'X
C     <error in user program; too many calc. parameters put into event>
       PARAMETER  ACQ_ERREVNTLEN = '08019024'X
C     <error detected in the control event length>
       PARAMETER  ACQ_EVTDESCIO = '0801902C'X
C     <i/o error by reading the event descriptor file>
       PARAMETER  ACQ_EVTDESCFMT = '08019034'X
C     <format error in the event descriptor file>
       PARAMETER  ACQ_EVTDESCLEN = '0801903C'X
C     <the lentgh of the event descriptor file is too big>
       PARAMETER  ACQ_PARDESCFMT = '08019044'X
C     <a format error was detected in the parameter table>
       PARAMETER  ACQ_CTRLIOEXIT = '0801904C'X
C     <the process ACQ_CTRL_IO exits now>
       PARAMETER  ACQ_UNKNODEID = '08019054'X
C     <the remote node identifier is unknown (symbol ACQ_REMOTE_NODE_ID)>
       PARAMETER  ACQ_CTRLUSRABT = '0801905C'X
C     <this abort error was signaled in the user part of control process>
       PARAMETER  ACQ_DRB32ERR = '08019064'X
C     <a fatal error was detected in the DRB32 driver...>
       PARAMETER  ACQ_ERROPNDECNET = '0801906C'X
C     <error occurs when opening a decnet task_to_task connection>
       PARAMETER  ACQ_ERRSPECLEN = '08019074'X
C     <error on length while read_spectrum operation>
       PARAMETER  ACQ_ERRLOCKSPEC = '0801907C'X
C     <fatal error on lock counter...please call manager>
       PARAMETER  ACQ_ERRALLOCSPEC = '08019084'X
C     <fatal error on mapping while read_spectrum operation>
       PARAMETER  ACQ_STOBUFTOOBIG = '0801908C'X
C     <the buffer size is too big to store under IN2P3 standard>
       PARAMETER  ACQ_FATPRCRASH = '08019094'X
C     <the acquisition process recently crashed>
       PARAMETER  ACQ_NOMORESTORE = '0801909C'X
C     <acquisition stopped because of tape or disk drive error>
       PARAMETER  ACQ_INVDATABUF = '080190A4'X
C     <This buffer is not a data buffer (not EVENTDB header)>
       PARAMETER  ACQ_ERRDATABUF = '080190AC'X
C     <An error was found in this data buffer >
       PARAMETER  ACQ_READCRASH = '080190B4'X
C     < Process ACQ_READ_DATA recently crashed>
       PARAMETER  ACQ_CTRLCRASH = '080190BC'X
C     < Process ACQ_CTRL_DATA process recently crashed>
       PARAMETER  ACQ_STORCRASH = '080190C4'X
C     < Process ACQ_STORE_DATA process recently crashed>
       PARAMETER  ACQ_ERRINIVME = '080190CC'X
C     < Error from VME during initialization phase>
       PARAMETER  ACQ_VMENOBUFREP = '080190D4'X
C     < TCPVME: no answer buffer and not enough memory, close connection>
       PARAMETER  ACQ_UNKNDATLINK = '080190DC'X
C     < Type of data link is unknown, see symbol ACQ_TYPE_OF_DATA_LINK>
       PARAMETER  ACQ_CENTRALERR = '080190E4'X
C     < a fatal error was detected in the central multiplexer crate...>
       PARAMETER  ACQ_NOREQSPACE = '080190EC'X
C     <not enough free space on disk to start a new run>
       PARAMETER  ACQ_DISKFULL = '080190F4'X
C     <no more disk space; current run stopped>
       PARAMETER  ACQ_TOOMCHINVDAT = '080190FC'X
C     <too much consecutive invalid buffers. Contact system manager >
       PARAMETER  ACQ_NOVMEBUFDISP = '08018643'X
C     <there is no ready buffer in VME crate>
       PARAMETER  ACQ_BUFINATTENDU = '08018648'X
C     <store (control) buffer when nostore (nocontrol) from VME>
       PARAMETER  ACQ_BUFTYPUNKNOWN = '08018650'X
C     <receive a buffer with an unknown type from VME>
       PARAMETER  ACQ_BUFTYPERR = '08018658'X
C     <control process receive a non control buffer>
       PARAMETER  ACQ_BUFVIDE = '08018660'X
C     <a new control buffer doesn't contain any event...>
       PARAMETER  ACQ_VMELBUFER = '08018668'X
C     <the received buffer has not the right length>
       PARAMETER  ACQ_FIFOEMPTY = '080188C3'X
C     <attempt to get from an empty queue>
       PARAMETER  ACQ_SIZTOOBIG = '080188C8'X
C     <queue size is too big>
       PARAMETER  ACQ_BUFSIZTOOBIG = '080188D0'X
C     <buffer size is too big>
       PARAMETER  ACQ_FIFOFULL = '080188D8'X
C     <attempt to put in queue which is already full>
       PARAMETER  ACQ_FIFODISAB = '080188E0'X
C     <attempt to put or get in queue while disabled>
       PARAMETER  ACQ_QNOEMPTY = '080188EA'X
C     <attempt to modify queue characteristics while not empty>
       PARAMETER  ACQ_BUFQNINIT = '080188F2'X
C     <buffer-queue not initialized>
       PARAMETER  ACQ_ERLOCKMGR = '080188FC'X
C     <error detected in lock manager...call system manager>
