#include <vector>
#include <functional>
#include "SVGigE.h"
#include "Camera.h"

SVSCAMERA_API class SVSCamera
{
private:
    SVSCamera(const SVSCamera&);
    const SVSCamera& operator=(const SVSCamera&);
 
    CameraContainerClient_handle m_CameraContainer;
    std::vector<Camera_handle> m_CameraList;
    std::vector<StreamingChannel_handle> m_StreamChannelList;
    //ͼ�����
    std::vector<int> m_ImgWidthList, m_ImgHeightList;
    std::vector<unsigned char*> m_RGBImgBuffer;
    //�������
    std::vector<float> m_ExposureTimeList;
    //ͼ��ص�
    //typedef std::function <SVGigE_RETURN (Image_handle, void*)> m_Callback;
    //union {
    //    void(__stdcall *ThreadProc)(void *, Image_handle, void*);
    //    void(__stdcall SVSCamera::*m_pImgCallback)(Image_handle, void*);
    //} Proc;
    //SVGigE_RETURN __stdcall m_ImgCallback(Image_handle, void*);

public:
    SVSCamera();
    ~SVSCamera();


    //1 ��ȡ����б�
    /*����һ��const��camera list��ʹ������Ҫ������֯�±�����*/
    const std::vector<std::string> getCameraList();
    //2 ��ָ�����
    /*����Ϊָ�������camera list�е��±�*/
    bool openCamera(std::vector<int>);
    ////3 ����ָ������ص�
    ///*����Ϊָ�������camera list�е��±��һ������ָ����ɵ�map*/
    //bool setCameraCB(std::map<int, CB_FUNC>);
    //4 ��ʼָ�����ץͼ
    /*����Ϊָ�������camera list�е��±�*/
    bool startGrab(std::vector<int>);
    //5 ָֹͣ�����ץͼ
    /*����Ϊָ�������camera list�е��±�*/
    bool stopGrab(std::vector<int>);
    //6 �ر�ָ�����
    /*����Ϊָ�������camera list�е��±�*/
    bool closeCamera(std::vector<int>);
    //7 ץȡ����ͼƬ
    bool singleGrab(std::vector<int>);
    //8 ��ȡBayer�任���RGBͼ��
    bool getRGBBuffer(std::vector<int>,unsigned char**);
    //9 ��ȡͼ���͸�
    bool getImgWidth(std::vector<int>,int&);
    bool getImgHeight(std::vector<int>,int&);
    //// �ر�ȫ�����������������ʱ�رմ򿪵������
    //bool closeCamera();

    //���ô���ģʽ
    bool setExposureTime(std::vector<int>,float);
};
