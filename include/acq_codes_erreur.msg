 .TITLE GANIL_ACQUISITION Errors and warnings messages
 .IDENT 'Version 1.0'
 .FACILITY ACQUIS,1/PREFIX=ACQ_


 .BASE 0
!*********************************************************************
!                zone messages interet general                       *
!                  (success et informational)                        *
!*********************************************************************

 .SEVERITY SUCCESS
OK <you know what? I'm HAPPY!>
ON <your acquisition is running>
OFF <your acquisition is halted>
ADDTAPEOK <successfull adding operation of a storage device >
REMTAPEOK <successfull removing operation of a storage device >
ENABTAPEOK <successfull enabling operation of storage device >
DISABTAPEOK <successfull disabling operation of storage device >
MOUNTTAPEOK < successfull mount operation >
INIACQEXEC <successfull execution of initialization of the VME crate>
           
 .SEVERITY INFORMATIONAL
TIMOUT <going out of "semwait" through time out condition>
MBXNOMSG <there is no message in that mailbox>
PURGEDPID <a communication session was purged...>
DBGMOD <starting debug mode for acquisition>
NODBGMOD <ending debug mode for acquisition>
CTRLMOD <starting control mode for acquisition>
FULLCTRLMOD <starting full control mode for acquisition>
STNDCTRLMOD <starting standard control mode for acquisition (no user call)>
USERCTRLMOD <starting user control mode for acquisition>
NOCTRLMOD <no more control is performed on acquisition>
STOREMOD <starting store mode for acquisition>
NOSTOREMOD <acquisition goes on without storage of datas>
SIMMOD <starting simulation mode for acquisition>
NOSIMMOD <ending simulation mode for acquisition>
CONNECTION <a new communication session starts now...>
DISCONNECT <a communication session stops!>
ASKFORABORT <received a demand to abort the application>
TAPEISFULL <tape was full,and dismounted, load another if necessary and mount it>
FREECTRLBUF <a buffer was freed up without control>
ENDOFBUFFER <the buffer doesn't contain any other event>
NOACQUIS <the GANIL acquisition is not started>
VISUOFFLINE <the visualisation runs off line>
VISUNOCONNECT <the visualisation is not actually connected to host>
FREEBUFONSTOP <when acquisition stopped, some buffers were freed up>
FREEBUFONCRASH <on crash of control process, some buffers were freed up>
ENDOFFILE <attempt to read past end of file>
ENDOFTAPE <attempt to read past end of tape>
DUMPALLMOD <starting dump all buffers mode>
DUMPNONEMOD <starting nodump buffers mode>
DUMPSTOREMOD <starting dump store buffers mode>
DUMPCTRLMOD <starting dump control buffers mode>
NEWCTRLTASK <starting with a new version of user control program>
NEWREADTASK <reload the read VME task (ACQ_READ_DATA)>
NEWSFROMVME <received from VME crate this informational message>
EXIT <ask to exit a command session>
REPLAYMOD <starting replay mode for acquisition>
NOREPLAYMOD <starting normal mode for acquisition>
NOMOREVINBUF <no more event in that buffer>
RETPROCCTRL <something has been sent back to ACQUISITION process>
READSTRT <starting ACQ_READ_DATA process>
CTRLSTRT <starting ACQ_CTRL_DATA process>
STORESTRT <starting ACQ_STORE_DATA process>
NOFILTDEF <no filter defined>
OPNOTCOMP <operation is not completed, wait a moment...>
VMEINIFILEXEC <auto execution of VME initialization file>
VMESTAFILEXEC <auto execution of VME start of run file>
VMEFIRFILEXEC <auto execution of VME "first in run" file>
VMEENDFILEXEC <auto execution of VME end of run file>
ARGALL <'all' character argument specified as command parameter>
NEWSTORETASK <reload the storage task (ACQ_STORE_DATA)>
AUTENDRUN <automatic end of run number >
STWHAUTENAB <stop acquisition (automatic mode is disabled, or no tape is available)>

 .BASE 300
!*********************************************************************
!                zone messages interet general                       *
!                         (warning)                                  *
!*********************************************************************
 
 .SEVERITY WARNING
NOTIMPL  <not yet implemented>
GLTOOSHORT <global section contains less bytes than requested>
NOCONNECT <got a message without connection code...>
NOMORESESSION <session table is full>
UNEXPMSG <the header of this message does not match with acquisition rules>
UNEXPCODE <the action code of this message is unknown >
NOPRIV <you don't have the required privilege for this operation!>
PRIVALRATTR <the privilege you want was previously attributed>
PRIVNOTALLOWED <the privilege you want requires a password>
ACTNOTAUT <this action is not authorized in this context>
TOOMUCHTAPE <you already have maximal allowed number of storage devices>
ALRALLOC <this storage device is already yours>
NOTALLOC <this storage device is not actually yours>
NOMORETAPE <you don't have any storage device for your acquisition>
NOTMOUNT <the tape on which you want to work is not mounted>
ALREADYMOUNT <the tape you want to mount is already mounted>
ISNOTATAPE <the name you enter doesn't represent a storage device>
TAPEINUSE <this storage device is actually used by your acquisition>
TAPENODISP <this storage device is not actually available>
BADEVENTFORM <the format of this event is not compatible with IN2P3 standard>
SPECTABFULL <you already got the maximal allowed count of spectra >
PARTABFULL <you already got the maximal allowed count of parameters >
DUPPLNAME <this name is already in this table>
UNKNSPEC <this spectrum was not defined in the table.Check name or number>
SPFULLSPACE <the whole space dedicated to spectra incrementation is used>
TOOMANYSPEC <too many spectra to start at the same time>
SPALRINC <some of these spectra are already incremented>
SPWRNGALRINC <some spectra to start were already being incremented>
SPCLRGALRINC <the spectra already incremented have been cleared>
SPNOTALLSUSP <some of the spectra you want to restart were not suspended>
SPNOTALLINC <some of the spectra you want to suspend are not started>
SPNOTINTABLE <the spectrum you want to replace is not in the dictionnary>
INCRSPEC <the spectrum you want to replace is actually incremented>
PARNOTINTABLE <the parameter you want to replace was not defined>
USEDPARAM <the parameter you want to replace is actually in use>
NOSUCHPAR <one parameter you want to use doesn't exist>
NOSUCHCOEF <the coefficient you want to change doesn't exist>
COEFTABFULL <you already got the maximal allowed count of user coefficients>
ERALLOCVM <virtual memory allocation failure >
NOTALLSTOPPED <all spectra cannot be stopped (remote connection ??)>
ERCODERSIZE <more bits were coded for this parameter than in description> 
STORNONEXIST <the spectrum you want save doesn't exist actually>
ERRLOGTAB <error detected in logical tables...>
NOSUCHSPEC <this spectrum was not defined>
NOINCRSPEC <this spectrum is not actually incremented>
CHRUNWHILACT <cannot change run number while acquisition runs>
CTRLUSRERR <this error was signaled in the user part of control process>
UNKNCOEF <this coeff was not defined in the table.Check name or number>
UNKNPARAM <this parameter was not defined in the table.Check name or number>
LOSTCONNECTION <sorry,lost connection with DECnet partner>
ACQALRRUN <Your acquisition is already running...>
INVPASSW <Invalid password>
DEVNOTMOUNT <The device you want to access is not mounted>
DEVWRITLOCK <This device is write-locked>
CTRLBUSY <The control process is busy...Try again later!>
NOCHGWHRUN <cannot reload any process while acquisition is running>
NOABTWHMOUN <Please dismount tapes before abort aquisition process>
ERVMESYNC < Some error occurs during synchronization with remote VME crate>
WARNFROMVME <received from VME crate this warning message>
NOINIWHRUN <cannot init VME crate while acquisition is running>
BUFALREADYGOT <current control buffer was already read from your task>
NOCTRLBUFDISP <there is currently no control buffer because acq is stopped >
ERRCTRLBUF <an error occurs in control buffer copy (twice changed)>
NOCURRCTRLBUF <there is no available ctrl buffer, please try again>
RAFRFULL  <auxilliary array for refreshment of spectrum is full>
WRBUFCTRL <will store a control buffer, please contact manager>
TOOMANYINCR <too many incremented spectra related to a parameter>
CNTOU_INVBITMP <invalid bitmap dimensions>
CNTOU_OUTBITMP <contour not within related bitmap>
CNTOU_TOOTWIST <too many intersections between contour and horizontal line>
CNTOU_FUNNY <something weird in the contour point list ; check it>
TOOMANYPOINTS <too many points in the current contour>
INVPOINTNUMB <invalid points number>
NOSUCHCNTOU <this contour was not defined>
INVCONTSCAL <invalid contour scaling factor>
INVCONTNAM  <invalid contour name>
CONTOUTABFULL <you already got the maximal allowed count of contours >
UNKNCONTOUR  <this contour was not defined in the table.Check name or number>
FAMINOCNTOU  <No contour belongs to this family. Check name>
CNTOUNOTIMPL  <type of contour not yet implemented>
BITMAPTABFULL <you already got the maximal allowed count of bitmaps >
NOBITMAP <No bitmap for this filter>
INVFILTNAM  <invalid filter name>
UNKNFILTER <this filter was not defined in the table.Check name or number>
FILTERTABFULL <you already got the maximal allowed count of filters >
INVFILTSYNTX <invalid filter syntax>
INVPARVAL <invalid parameter value specified>
DUPPLFILTER  <this filter is already in the table>
TOOLONGFILTDEF <too long filter def. string; split it into several filters>
NOFILDELWHCOND <Warning ! A spectrum is conditionned by this filter !>
NOFILDELWHCOMP <Warning ! This filter is used to define a compound filter !>
INVFILESPEC <invalid file specification>
NOTAPACTWHRUN <skip actions on tape are not allowed while running>
VMEINIFILNOFND <VME initialization file not found for auto execution>
VMESTAFILNOFND <VME start of run file not found for auto execution>
VMEFIRFILNOFND <VME "first in run" file not found for auto execution>
VMEENDFILNOFND <VME end of run file not found for auto execution>
NONSTWHMOUN <Please dismount tapes before reload storage process>
NOMODCHGWHRUN <buffer selection mode is not allowed while acquisition is running>
                                       
 .BASE 400
!*********************************************************************
!                zone messages interet general                       *
!                         (error)                                    *
!*********************************************************************

 .SEVERITY ERROR
ERRPARAM <parameter error>
VMAXTBIG <maximal value out of range>
VMINTBIG <minimal value out of range>
VTOOBIG  <value out of range>
CNXREFUS <connection to "acquisition" refused>
TOOMUCHCHAR <too much characters in this definition >
RAFBUFOVF <overflow of the refresh buffer>
NOREPCTRL <the ACQ_CTRL_DATA process doesn't answer...sorry!>
IOFORT <i/o error detected in fortran calls>
INVARG <invalid or missing argument(s) in your command>
ERRDURREAD <error occurs while reading a tape or disk file>
ERRNUMSPEC <call to acq_inc_user... routines for unknown spectra>
ERRTYPSPEC <call to acq_inc_user... routines for spectra of another type>
ERRSIZSPEC <call to acq_inc_user... with an index larger than the spectra size>
BADTAPEFORM <format of tape or disk file is not compatible with IN2P3 standard>
ERRFROMVME <a fatal error message was issued from VME crate>
VMECOMLEN <Length of vme command line too big>
TAPEISVERYFULL <problem of synchro with VME, and must close the file on tape>
VMETCPNOMEM < TCPVME: not enough memory >
TCPREPTOOSMALL < TCPVME: answer buffer is too small>
IOFAMFILE <Input/Output error with the spectrum family file>
ERRSETSYMBFAM <Cannot set the symbol for one spectra family>
STOSPECIO <i/o error in storage operation of spectra>
STOCONTOURIO <i/o error in storage operation of contours>
READCONTOURIO <i/o error in reading operation of contours>
ERRFILTFILEIO <error in filter file input/output>
INVDATINBUF <a buffer contains invalid datas, contact system manager>
NODEVMOUNT <no storage operation is possible because no device is mounted>

 .BASE 500
!*********************************************************************
!                zone messages interet general                       *
!                        (fatal)                                     *
!*********************************************************************

 .SEVERITY FATAL
NODLINVALID <the ODL'number is invalid (not initialized or false)>
READEXIT <the process ACQ_READ_DATA exits now>
STOREEXIT <the process ACQ_STORE_DATA exits now>
CTRLEXIT <the process ACQ_CTRL_DATA exits now>
ACQUISEXIT <the process ACQUISITION exits now>
PRCRASH <one of the acquisition process recently crashed>
FATERRREC < did receive a fatal error code message >
ERRFICCOEF <error detected in the initialization file of user coefficients>
ERRFICSPECTRE <error detected in the initialization file of spectra>
ERRFICPARBR <error detected in the initialization file of parameters>
ERRFICPARCA <error detected in the initialization file of calculated parameters>
ERRDICOSPEC <error detected in spectra's description tables>
ERRDICOPAR <error detected in parameters' description tables>
ERRDICOCNTOU <error detected in contours' description tables>
ERRDICOFILT <error detected in filters' description tables>
ERREVTCALC <error in user program; too many calc. parameters put into event>
ERREVNTLEN <error detected in the control event length>
EVTDESCIO <i/o error by reading the event descriptor file>
EVTDESCFMT <format error in the event descriptor file>
EVTDESCLEN <the lentgh of the event descriptor file is too big>
PARDESCFMT <a format error was detected in the parameter table>
CTRLIOEXIT <the process ACQ_CTRL_IO exits now>
UNKNODEID <the remote node identifier is unknown (symbol ACQ_REMOTE_NODE_ID)>
CTRLUSRABT <this abort error was signaled in the user part of control process>
DRB32ERR <a fatal error was detected in the DRB32 driver...>
ERROPNDECNET <error occurs when opening a decnet task_to_task connection>
ERRSPECLEN <error on length while read_spectrum operation>
ERRLOCKSPEC <fatal error on lock counter...please call manager>
ERRALLOCSPEC <fatal error on mapping while read_spectrum operation>
STOBUFTOOBIG <the buffer size is too big to store under IN2P3 standard>
FATPRCRASH <the acquisition process recently crashed>
NOMORESTORE <acquisition stopped because of tape or disk drive error>
INVDATABUF <This buffer is not a data buffer (not EVENTDB header)>
ERRDATABUF <An error was found in this data buffer >
READCRASH < Process ACQ_READ_DATA recently crashed>
CTRLCRASH < Process ACQ_CTRL_DATA process recently crashed>
STORCRASH < Process ACQ_STORE_DATA process recently crashed>
ERRINIVME < Error from VME during initialization phase>
VMENOBUFREP < TCPVME: no answer buffer and not enough memory, close connection>
UNKNDATLINK < Type of data link is unknown, see symbol ACQ_TYPE_OF_DATA_LINK>
CENTRALERR < a fatal error was detected in the central multiplexer crate...>
NOREQSPACE <not enough free space on disk to start a new run>
DISKFULL <no more disk space; current run stopped>
TOOMCHINVDAT <too much consecutive invalid buffers. Contact system manager >

 .BASE 200
!*********************************************************************
!       codes concernant la lecture (process "ACQ_READ_DATA")        *
!*********************************************************************
 .SEVERITY INFORMATIONAL
NOVMEBUFDISP <there is no ready buffer in VME crate>

 .SEVERITY WARNING
BUFINATTENDU <store (control) buffer when nostore (nocontrol) from VME>
BUFTYPUNKNOWN <receive a buffer with an unknown type from VME>
BUFTYPERR <control process receive a non control buffer>
BUFVIDE <a new control buffer doesn't contain any event...>
VMELBUFER <the received buffer has not the right length>

  .BASE 280
!*********************************************************************
!                          gestion buffer                            *
!*********************************************************************
 .SEVERITY INFORMATIONAL

FIFOEMPTY <attempt to get from an empty queue>

 .SEVERITY WARNING
SIZTOOBIG    <queue size is too big>
BUFSIZTOOBIG  <buffer size is too big>
FIFOFULL     <attempt to put in queue which is already full>
FIFODISAB    <attempt to put or get in queue while disabled>
 .SEVERITY ERROR
QNOEMPTY     <attempt to modify queue characteristics while not empty>
BUFQNINIT    <buffer-queue not initialized>
 .SEVERITY FATAL
ERLOCKMGR <error detected in lock manager...call system manager>


 .END
