//
// Created by xuan on 17-5-25.
//

#include "PaperFun.h"

void on_paper::PaperFun::init(int page) {
    string json_name,json_str="",str;
    json_name=CONFIGPATH+utils::into_name(page,ZERONUM)+".json";
    //cout<<json_name<<endl;
    ifstream json_file(json_name);
    if(!json_file.is_open())
    {
        cout<<"json file open failed"<<endl;
        return;
    };
    while(!json_file.eof())
    {
        if(json_file.eof())
            break;
        json_file>>str;
        json_str.append(str);
    }
    //cout<<json_str<<endl;
    j=json::parse(json_str);
    parseJson();

    appended_fun_layer.zeros(800,600,CV_8UC3);
    return;
}

//must ensure the transmatrix is not empty first.
void on_paper::PaperFun::transform_point(Point& p){
    vector<Point2f> mreal = {p};
    vector<Point2f> mimage;
    perspectiveTransform(mreal,mimage, this->transmatrix );
    p = mimage[0];
}

void on_paper::PaperFun::showPic(vector<Point> figPs) {
    string pic_path="";
    Point figP;
    for(vector<Point>::iterator it=figPs.begin();it!=figPs.end();it++)
    {
        figP=*it;
        transform_point(figP);
        for(vector<Info>::iterator iter=json_parse.begin();iter!=json_parse.end();iter++)
        {
            if(figP.x > iter->tl.x and figP.x < iter->br.x and figP.y < iter->br.y and figP.y > iter->tl.y )
            {
                if(pic_path!=iter->pic_path)
                {
                    pic_path=iter->pic_path;
                    picture=imread(pic_path);

                    //cout<<pic_path<<endl;

                    Mat resizedPic;
                    resize(picture,resizedPic,cv::Size(200,(float)200/picture.cols*picture.rows));
                    picture=resizedPic;

                    cv::imshow("pic",picture);

                }

            }
        }

    }
}

void on_paper::PaperFun::parseJson() {
    Info info;
    string str;
    for(json::iterator it = j.begin(); it!=j.end(); it++)
    {
        info.tl=Point(it.value().at("tlx").get<int>(),it.value().at("tly").get<int>());
        info.br=Point(it.value().at("brx").get<int>(),it.value().at("bry").get<int>());
        info.pic_path=it.value().at("pic").get<std::string>();
        json_parse.push_back(info);
    }
    /*
    for(vector<Info>::iterator iter=json_parse.begin();iter!=json_parse.end();iter++)
    {
        cout<<iter->tl<<iter->br<<iter->pic_path<<endl;
    }
    */
}
