//
// Created by xuan on 17-5-25.
//

#include "PaperFun.h"


void on_paper::PaperFun::load_archiv_conf(archiv_conf acnf)
{

    this->pic_name="";
    this->aconf=acnf;
    ifstream json_file(aconf.conf_path);
    //clear
    json_parse.clear();
    j.clear();

    if(!json_file.is_open())
    {
        cout<<"json file open failed"<<endl;
        return;
    };

    try{
        while(true)
        {
            json_file>>j;
            parseJson();
        }
    }
    catch(std::exception & e){

    }

}

void on_paper::PaperFun::parseJson()
{

    Info info;
    vector<Info> v_info;
    int page;

    page=j["page"];
    //cout<<page<<endl;
    //cout<<j["info"]<<endl;
    v_info.clear();
    for( json::iterator it = j["info"].begin(); it!=j["info"].end(); it++)
    {
        info.tl=Point(it.value().at("tlx").get<int>(),it.value().at("tly").get<int>());
        info.br=Point(it.value().at("brx").get<int>(),it.value().at("bry").get<int>());
        info.function=it.value().at("fun").get<std::string>();
        info.data=it.value().at("addition").get<std::string>();
        v_info.push_back(info);
    }
    json_parse.insert(pair<int,vector<Info>>(page,v_info));

    /*
    vector<Info> x_info;
    for(vector<Info>::iterator iter=v_info.begin();iter!=v_info.end();iter++)
    {
        cout<<iter->tl<<iter->br<<iter->data<<endl;
    }

    x_info=json_parse[5];
    for(vector<Info>::iterator iter=x_info.begin();iter!=x_info.end();iter++)
    {
        cout<<iter->tl<<iter->br<<iter->data<<"!!!!"<<endl;
    }
    */
}


//must ensure the transmatrix is not empty first.
void on_paper::PaperFun::transform_point(Point& p){
    vector<Point2f> mreal = {p};
    vector<Point2f> mimage;
    perspectiveTransform(mreal,mimage, this->transmatrix );
    p = mimage[0];
}

void on_paper::PaperFun::fire_event(vector<Point> figPs, Point &figP,int page) {

    msg="NIL";
    figP=Point(0,0);

    if(!j.empty())
    {

        for(vector<Point>::iterator it=figPs.begin();it!=figPs.end();it++)
        {
            figP=*it;
            transform_point(figP);
            vector<Info> v_info;
            v_info=json_parse[page];
            for(vector<Info>::iterator iter=v_info.begin();iter!=v_info.end();iter++)
            {

                Point trans_tl,trans_br;
                trans_tl=pixtransform(iter->tl);
                trans_br=pixtransform(iter->br);

                //pa_ptr->draw_enlarged_rect(Rect(trans_tl, trans_br));
                //pa_ptr->draw_rect(Rect(trans_tl, trans_br), Scalar(0,0,255), 4);
                if(judgeIn(figP,trans_tl, trans_br))
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

bool on_paper::PaperFun::contains_triggers(int page)
{
    vector<Info> v_info;
    v_info=json_parse[page];
    return (not v_info.empty());
}


bool on_paper::PaperFun::judgeIn(cv::Point p, cv::Point tl, cv::Point br) {
    return p.x > tl.x and p.x < br.x and p.y < br.y and p.y > tl.y;
}
#define PAPERWIDTH 2480
#define PAPERHEIGHT 3508
cv::Point on_paper::PaperFun::pixtransform(cv::Point &p)
{
    int canvas_col,canvas_row;
    Point t_p;
    canvas_col=pa_ptr->get_temp_canvas().cols;//get_canvas_layer().cols;
    canvas_row=pa_ptr->get_temp_canvas().rows;//get_canvas_layer().rows;
    t_p.x=(float)p.x/PAPERWIDTH*canvas_col;
    t_p.y=(float)p.y/PAPERHEIGHT*canvas_row;
    return t_p;
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
                if(pic_name!=i.data)
                {
                    pic_name=i.data;
                    picture=imread(aconf.source_path+pic_name);
                    Mat resizedPic;
                    resize(picture,resizedPic,cv::Size(1000,(float)1000/picture.cols*picture.rows));
                    picture=resizedPic;
                }
                pa_ptr->paste_temp_pic(picture, i.finger);
                msg="Found secret area.";
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
                msg="Picked up color."     ;
            }
    ));
    this->_fnmap.insert(make_pair<string, functor>(
            "toolbox",
            [&](Info i){
                auto tool = i.data;
                if(tool == "pencil") {
                    pa_ptr->set_color(Scalar(255, 255, 255));
                    pa_ptr->set_pen_size(10);
                    msg="Picked up pencil.";
                }
                else if(tool == "eraser") {
                    pa_ptr->set_color(Scalar(0, 0, 0));
                    pa_ptr->set_pen_size(200);
                    msg="Picked up eraser.";
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
                msg="Renewed pen size.";
            }
    ));

}

