//
// Created by xuan on 17-5-25.
//

#include "PaperFun.h"

void on_paper::PaperFun::init(int page) {
    pic_path = "";
    string json_name,json_str="",str;
    json_name=CONFIGPATH+utils::into_name(page,ZERONUM)+".json";
    cout<<json_name<<endl;
    ifstream json_file(json_name);

    //clear
    json_parse.clear();
    j.clear();
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

   // appended_fun_layer.zeros(800,600,CV_8UC3);
    return;
}

//must ensure the transmatrix is not empty first.
void on_paper::PaperFun::transform_point(Point& p){
    vector<Point2f> mreal = {p};
    vector<Point2f> mimage;
    perspectiveTransform(mreal,mimage, this->transmatrix );
    p = mimage[0];
}

void on_paper::PaperFun::fire_event(vector<Point> figPs, Point &figP) {
    figP=Point(0,0);
    if(!j.empty())
    {
        for(vector<Point>::iterator it=figPs.begin();it!=figPs.end();it++)
        {
            figP=*it;
            transform_point(figP);
            for(vector<Info>::iterator iter=json_parse.begin();iter!=json_parse.end();iter++)
            {
                if(judgeIn(figP,iter->tl,iter->br))
                {
                    string function = iter->function;
                    iter->finger = figP;
                    //take that elegance.
                    call_paper_fun(function, *iter);

                    break;
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
        info.function=it.value().at("fun").get<std::string>();
        info.data=it.value().at("addition").get<std::string>();
        json_parse.push_back(info);
    }
    /*
    for(vector<Info>::iterator iter=json_parse.begin();iter!=json_parse.end();iter++)
    {
        cout<<iter->tl<<iter->br<<iter->pic_path<<endl;
    }
    */
}

bool on_paper::PaperFun::judgeIn(cv::Point p, cv::Point tl, cv::Point br) {
    return p.x > tl.x and p.x < br.x and p.y < br.y and p.y > tl.y;
}

void on_paper::PaperFun::call_paper_fun(string function_name, Info arg) {
    auto bind = this->_fnmap.find(function_name);
    if(bind != this->_fnmap.end()) //gotcha!!
    {
        //let's rock!
        bind->second(arg); //call the paper function.
    } else {
        cout<<"No such binding!"<<endl; //an ad-hoc debugger.
    }
}

void on_paper::PaperFun::register_callbacks(void) {
    this->_fnmap.insert(make_pair<string, functor>(
            "addpicture",
            [&](Info i){
                if(pic_path!=i.data)
                {
                    pic_path=i.data;
                    picture=imread(string(PICTUREPATH)+pic_path);
                    Mat resizedPic;
                    resize(picture,resizedPic,cv::Size(1000,(float)1000/picture.cols*picture.rows));
                    picture=resizedPic;
                }
                pa_ptr->paste_temp_pic(picture, i.finger);
                pa_ptr->text_broadcast("Displaying elements.");
            }
    ));
    this->_fnmap.insert(make_pair<string, functor>(
            "setcolor",
            [&](Info i){
                string _b, _g, _r;
                int b,g,r;
                stringstream ss(i.data);
                getline(ss, _b, ',');
                getline(ss,_g,',');
                getline(ss,_r,',');
                b = atoi(_b.data());g=atoi(_g.data());r=atoi(_r.data());
                pa_ptr->set_color(Scalar(b,g,r));
                pa_ptr->text_broadcast("Picked up color.");
            }
    ));
    this->_fnmap.insert(make_pair<string, functor>(
            "toolbox",
            [&](Info i){
                auto tool = i.data;
                if(tool == "pencil") {
                    pa_ptr->set_color(Scalar(255, 255, 255));
                    pa_ptr->set_pen_size(10);
                    pa_ptr->text_broadcast("Picked up pencil.");
                }
                else if(tool == "eraser") {
                    pa_ptr->set_color(Scalar(0, 0, 0));
                    pa_ptr->set_pen_size(200);
                    pa_ptr->text_broadcast("Picked up eraser.");
                } else {
                    cout<<"Not implemented."<<endl;
                }
            }
    ));

    this->_fnmap.insert(make_pair<string, functor>(
            "setwidth",
            [&](Info i){
                auto width = i.data;
                pa_ptr->set_pen_size(atoi(width.data()));
                pa_ptr->text_broadcast("Renewed pen size.");
            }
    ));

}

