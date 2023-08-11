#include "hms2000_thread.h"
#include "definition.h"

hms2000_thread::hms2000_thread(QObject *parent): QThread(parent)
{
    qDebug()<<"创建 hms2000_thread";
    isPlay = false;
    stopped = false;

    image1 = new QImage(800, 700, QImage::Format_RGB32);
    image1->fill(Qt::white);
    m_pBits = (QRgb *)image1->bits();

    param_init();
    calDelta();
    InitColortable();


    //绘制颜色表
    DrawColortable(m_pBits);

    hmsfile.setFileName("hmsrecv.txt");
    hmsfile.open(QIODevice::WriteOnly|QIODevice::Truncate); //以重写的方式先清空
    hmsfile.close();

    //创建 QUdpSocket 对象
    m_udp = new QUdpSocket(this);
    m_udp->bind(QHostAddress::Any, 6666);

    //创建命令下发定时器
    simu_cmd();//初始化命令包
    sendbytearray.append((char *)(&simucmd),sizeof(simucmd));

    cmdTimer = new QTimer;
    cmdTimer -> start(100); //定时发送命令包
}

void hms2000_thread::param_init()
{
    m_nAngle = 0;
    m_nLastAngle = -1;
    m_nMaxAngle = 588;
    hms_bufnow = hms_buf1;
    //m_scan_step = 2*360./m_nMaxAngle;//步距4
    m_scan_step = 360./m_nMaxAngle;//步距2
    //m_scan_step = 0.5*360./m_nMaxAngle;//步距1
    m_width = image1->width();
    m_height = image1->height();
    m_radius = (m_width>m_height?m_height:m_width)/2.0;//半径
    m_nOrgX = m_width/2;
    m_nOrgY = m_height/2;
}

//计算HMS2000绘制声呐图像时 不同角度半径增加1时 x,y坐标变化值
void hms2000_thread::calDelta()
{
    double Q = -90;
    double dQ;

    m_nScanLineMax = 4800;
    m_nScanPerAngle = 14;

    dQ = 360./(double)m_nScanLineMax;
    for( uint i=0;i<m_nScanLineMax;i++ )
    {
        double dx = cos( Q*M_PI/180. );
        double dy = sin( Q*M_PI/180. );

        m_dwDeltaX[i] = (uint32_t)(dx*65536+0.5);
        m_dwDeltaY[i] = (uint32_t)(dy*65536+0.5);
        Q += dQ;
    }
}

//从bin文件中读取设置好的颜色表
void hms2000_thread::InitColortable()
{
    QFile file(":/config/config/ctable.bin");
    file.open(QIODevice::ReadOnly);
    uchar data[768];

    file.read((char*)data,768);

    for(int i=0;i<256;i++)
    {
        uint a =0xff000000+((data[i*3]<<16)+(data[i*3+1]<<8)+(data[i*3+2]));
        *(m_mycolorTable+i)=a;
    }
    file.close();
}

void hms2000_thread::DrawColortable(QRgb *bits)
{
    int index;
    int width = image1->width();
    int high = image1->height();
    for (int y = 1; y <= 256; ++y)
    {
        for (int x=width-10; x<width ; x++)
        {
            index = (high-y)*width + x;
            uint r = (m_mycolorTable[y-1]>>16)&0x000000ff;
            uint g = (m_mycolorTable[y-1]>>8)&0x000000ff;
            uint b = (m_mycolorTable[y-1])&0x000000ff;

            *(bits+index) = qRgb(r,g,b);
        }
    }
}

void hms2000_thread::start_receive_data()
{

}

void hms2000_thread::process_receive_data()
{
    QByteArray arr;
    arr.resize(m_udp->bytesAvailable());
    int recvlen = arr.size();
    m_udp->readDatagram(arr.data(),recvlen); //接收
    if(!hmsfile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))//Append以追加的方式
    {
        qDebug()<<"打开失败";
    }
//    uchar outbuf[recvlen];
     char *outbuf = new char[recvlen];

    memcpy(outbuf,arr.data(),recvlen);
    QTextStream out(&hmsfile);
    for(uint i=0; i<recvlen; i++)
    {
        out<<outbuf[i]<<" ";
    }
    out<<Qt::endl;
    out.flush();
    hmsfile.close();
    //qDebug()<<arr.data();
    //qDebug()<<"实时数据流接收到了"<<recvlen;

    if(recvlen % ANSPKTSIZE == 0)
    {
        int cnt = 0;
        while (recvlen > 0)
        {
            char buf[ANSPKTSIZE];
            memcpy(buf,arr.data()+cnt*ANSPKTSIZE,ANSPKTSIZE);
            recvlen -= ANSPKTSIZE;
            cnt++;

            int tail = 0;
            uchar tempanshead[sizeof(ANSHEAD)] = {0};
            for(uint i = 0;i < sizeof(ANSHEAD);i++)
            {
                tempanshead[i] = (uchar) buf[tail];
                tail++;
            }
            memcpy(&anshead, tempanshead, sizeof(ANSHEAD));

            //如果是高度计
            if((anshead.ucDevice == Device_STA455) && (anshead.ucFlag == 0x81))
            {
                unsigned char check = (anshead.ucID+anshead.ucStatus+anshead.ucDataLo+anshead.ucDataHi)&0x7F;
                if(check == anshead.ucAngleLo)
                {
                    RTHeight = (int)((anshead.ucID-0x30)*1000+(anshead.ucStatus-0x30)*100+
                                      (anshead.ucDataLo-0x30)*10+(anshead.ucDataHi-0x30));
                    //触发信号
                    //qDebug()<<"高度："<<RTHeight;
//                    ui->rtheight->setText(QString::number(RTHeight,10));
                }
                //qDebug()<<"处理一个高度计包，剩余有效数据字节数为";
            }
            //如果是侧扫声纳或底扫声纳
            else if(((anshead.ucDevice == Device_HMS2000)||(anshead.ucDevice == Device_STD46TS))
                     &&(anshead.ucFlag == 0xFE)&&(anshead.ucEnd == 0xFD))
            {
                uchar device_type = anshead.ucDevice;
                switch (device_type)
                {
                case Device_HMS2000:
                {
                    sonardatahms2000_in.data_len = (((anshead.ucDataHi&0x7f)<<7)|(anshead.ucDataLo&0x7f));
                    sonardatahms2000_in.glb_nAngle = (((anshead.ucAngleHi&0x7f)<<7)|(anshead.ucAngleLo&0x7f));
                    for(unsigned int i = 0;i < sonardatahms2000_in.data_len;i++)
                    {
                        sonardatahms2000_in.sonardata_hms2000[i] = buf[tail];
                        tail++;
                    }
                    q_sonardatahms2000.push(sonardatahms2000_in);
                    q_sonardatahms2000_num++;

                    //qDebug()<<"处理了一个HMS2000包，数据区长度为："<<sonardatahms2000_in.data_len;
                    break;
                }
//                    case Device_STD46TS:
//                    {
//                        sonardatastd46ts_in.data_len = (((anshead.ucDataHi&0x7f)<<7)|(anshead.ucDataLo&0x7f))-8;
//                        sonardatastd46ts_in.glb_nAngle = (((anshead.ucAngleHi&0x7f)<<7)|(anshead.ucAngleLo&0x7f));
//                        tail+=8;
//                        for(unsigned int i = 0;i < sonardatastd46ts_in.data_len;i++)
//                        {
//                            sonardatastd46ts_in.sonardata_std46ts[i] = buf[tail];
//                            tail++;
//                        }
//                        q_sonardatastd46ts.push(sonardatastd46ts_in);
//                        q_sonardatastd46ts_num++;

//                        //qDebug()<<"处理了一个STD46TS包，数据区长度为："<<sonardatastd46ts_in.data_len;
//                        break;
//                    }
                }
            }

        }
    }
    else
    {
        qDebug()<<"接收失败"<<recvlen;
    }
}

//HMS2000图像绘制  角度angle测得的一组数据buf 一组数据长度len=300
//步距为4，需要绘制的角度宽度为step=1.224° 实测数据每2.448°得到一组 由于前后两组平均后最后的步距角为1.224°
//步距为1，需要绘制的角度宽度为step=0.306° 实测数据每0.612°得到一组 由于前后两组平均后最后的步距角为0.306°
void hms2000_thread::Scan(double angle, uchar *buf, uint len, double step)
{
    //qDebug()<<angle<<endl;
    int index;

    //if( !hDIB ) return;
    while( angle>=360.0 ) angle-=360.0;

    int scans = (int)(m_nScanPerAngle*step)+1;

    int idx = (int)(angle*m_nScanLineMax/360.0 - scans/2);

    if( idx<0 ) idx += m_nScanLineMax;

    if( len>m_nR ) len = m_nR;

    for( int i=0;i<scans;i++ )
    {
        uint32_t x1 = m_nOrgX*65536;
        uint32_t y1 = m_nOrgY*65536;

        for( uint j=0;j<len;j++ )
        {
            x1 += m_dwDeltaX[idx];
            y1 += m_dwDeltaY[idx];

            uint x = (uint)0x0000ffff&(x1>>16);
            uint y = (uint)0x0000ffff&(y1>>16);

            if( x>=m_width||y>=m_height ) continue;
            if( x<0||y<0 ) continue;

            index = (y)*m_width + x;

            uint r = (m_mycolorTable[buf[j]*2]>>16)&0x000000ff;
            uint g = (m_mycolorTable[buf[j]*2]>>8)&0x000000ff;
            uint b = (m_mycolorTable[buf[j]*2])&0x000000ff;

            *(m_pBits+index) = qRgb(r,g,b);
        }
        idx++;
        //qDebug()<<idx<<endl;
        if( (uint)idx>=m_nScanLineMax ) idx = 0;
    }
}

void hms2000_thread::start_sonar()
{
    simucmd.HMS2000.ucWorkStatus = 0x03;
    updateCheckSum();
    sendbytearray.clear();
    sendbytearray.append((char *)(&simucmd),sizeof(simucmd));
    m_udp->writeDatagram(sendbytearray,QHostAddress(connectAdd),connectPort); //发送
}

void hms2000_thread::stop_sonar()
{
    simucmd.HMS2000.ucWorkStatus = 0x43;
    updateCheckSum();
    sendbytearray.clear();
    sendbytearray.append((char *)(&simucmd),sizeof(simucmd));
    m_udp->writeDatagram(sendbytearray,QHostAddress(connectAdd),connectPort); //发送
}

void hms2000_thread::run()
{

    qDebug()<<"hms2000_thread run";

//    connect(cmdTimer, &QTimer::timeout,this,[=]()
//    {
//        if(!stopped){
//            m_udp->writeDatagram(sendbytearray,QHostAddress(connectAdd),connectPort); //发送
//            qDebug()<<"发送一次udp指令";
//        }
//    });

    connect(m_udp, &QUdpSocket::readyRead, this,&hms2000_thread::process_receive_data);

    while (!stopped) {
        //根据标志位执行初始化操作
        if (isPlay) {
            isPlay = false;
            continue;
        }

        while (q_sonardatahms2000_num)
        {
            q_sonardatahms2000.wait_and_pop(sonardatahms2000_out);
            q_sonardatahms2000_num--;
            m_nAngle = sonardatahms2000_out.glb_nAngle;
            memcpy(hms_bufnow,sonardatahms2000_out.sonardata_hms2000,sonardatahms2000_out.data_len);
            if(m_nLastAngle!=-1)
            {
                int i;
                for(i=0;i<300;i++)
                {
                    hms_midBuf[i] = (hms_buf1[i]+hms_buf2[i])/2;
                }
                double midangle = 0;  //int类型无法计算中间角度
                if(abs(m_nAngle-m_nLastAngle)>100)
                {
                    midangle=(m_nAngle+m_nLastAngle+m_nMaxAngle)/2;
                    while (i>m_nMaxAngle)  midangle -= m_nMaxAngle;
                }
                else
                {
                    midangle = (m_nAngle+m_nLastAngle)/2.0;
                }
                m_angle = midangle*360.0/m_nMaxAngle;  //将此角度作为中心角度，绘制m_scan_step
                Scan(m_angle,hms_midBuf,sonardatahms2000_out.data_len,m_scan_step);
            }
            m_angle = m_nAngle*360.0/m_nMaxAngle;
            Scan(m_angle,hms_bufnow,sonardatahms2000_out.data_len,m_scan_step);

            if(hms_bufnow == hms_buf1)
            {
                hms_bufnow = hms_buf2;
            }
            else
            {
                hms_bufnow = hms_buf1;
            }
            m_nLastAngle = m_nAngle;
            emit receiveImage(*image1);
        }


    }

    //线程结束后释放资源
    stopped = false;
    isPlay = false;

}

void hms2000_thread::set(QString sonar_add, quint16 sonar_port)
{
    connectAdd = sonar_add;
    connectPort = sonar_port;
}

void hms2000_thread::play()
{
    this->isPlay = true;
}

void hms2000_thread::stop()
{
    stopped = true;
}



