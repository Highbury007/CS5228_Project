//
//  ProbeRatings.cpp
//  CS5228Project
//
//  Created by OrangeR on 26/11/13.
//  Copyright (c) 2013 OrangeR. All rights reserved.
//

#include "ProbeRatings.h"

#include <ctime>
#include <utility>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "Helper.h"
using namespace std;
ProbeRatings::ProbeRatings():Ratings()
{
    realRatings = new std::vector< std::vector<mRatings>* >();
    predictedRatings = new std::vector<double>();
    //isPredicted = false;
}

ProbeRatings::~ProbeRatings()
{
    for(std::vector< std::vector<mRatings>* >::iterator it = realRatings->begin();
        it != realRatings->end(); ++ it)
    {
        std::vector<mRatings>* pTemp = *it;
        pTemp->clear();
        delete pTemp;
    }
    realRatings->clear();
    delete realRatings;
    predictedRatings->clear();
    delete predictedRatings;
}

uInt ProbeRatings::getPredictedMovieNum()
{
    return uInt(mIds.size());
}

void ProbeRatings::addRatings(double &prediction)
{
    predictedRatings->push_back(prediction);
}

void ProbeRatings::addRatings(uShort movieId, std::vector<mRatings> &pRs)
{
    std::vector<mRatings>* pTemp = new std::vector<mRatings>();
    for(std::vector<mRatings>::iterator it = pRs.begin(); it != pRs.end(); ++ it)
    {
        pTemp->push_back(*it);
    }

    mIds.push_back(movieId);
    realRatings->push_back(pTemp);
}

void ProbeRatings::getRatings(uInt idx, uShort &movieId, std::vector<mRatings> &pRs)
{
    
    if(idx > mIds.size())
    {
        std::cerr << "ProbeRatings::getRatings, index out of range!" << std::endl;
        return ;
    }
    
    if(pRs.size() != 0)
    {
        std::cerr << "the reference of vector<mRatings> should be empty" << endl;
        exit(0);
    }
    //pRs.clear();
    movieId = mIds.at(idx);
    for(std::vector<mRatings>::iterator it = realRatings->at(idx)->begin(); it != realRatings->at(idx)->end(); ++ it)
    {
        pRs.push_back(*it);
    }
}

void ProbeRatings::dumpAllRatings()
{
    for(int i = 0; i < mIds.size(); i ++)
    {
        dumpRatings(i);
    }
}

void ProbeRatings::dumpRatings(uInt idx)
{
    if(idx > mIds.size() - 1)
    {
        std::cerr << "ProbeRatings::dumpRatings, index out of range!" << std::endl;
        return ;
    }
    else
    {
        std::cout << "MovieId " << mIds.at(idx) << ":" << std::endl;
        for(std::vector<mRatings>::iterator it = realRatings->at(idx)->begin(); it != realRatings->at(idx)->end(); ++ it)
        {
            std::cout << it->getId() << "," << it->getValue() << std::endl;
        }
    }
}

double ProbeRatings::RMSE()
{
    uInt num = 0;
    double rmse = 0;
    for(int i = 0; i < realRatings->size(); i ++)
    {
        num += realRatings->at(i)->size();
    }
    
    if(num != predictedRatings->size())
    {
        std::cerr << "Prediction is incomplete!" << std::endl;
        return -1;
    }
    else
    {
        uInt index = 0;
        for(int i = 0; i < realRatings->size(); i ++)
        {
            for(std::vector<mRatings>::iterator it = realRatings->at(i)->begin();
                it != realRatings->at(i)->end(); ++ it)
            {
                rmse += pow(predictedRatings->at(index) - it->getValue(), 2);
                index ++;
            }
        }
        //savePredictions(PREDICTION_PATH, genTimeStamp() + ".txt");
        predictedRatings->clear();
        return m_rmse = sqrt(rmse / num);
    }
}

void ProbeRatings::savePredictions(const std::string &path, const std::string &file_name)
{
    
    uInt num = 0;
    for(int i = 0; i < realRatings->size(); i ++)
    {
        num += realRatings->at(i)->size();
    }
    
    if(num != predictedRatings->size())
    {
        std::cerr << "Prediction is incomplete!" << std::endl;
        return;
    }
    else
    {
        //check the directory
    
        DIR *dp;
        if((dp = opendir(path.c_str())) == NULL){
            if(mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
            {
                cout << "mkdir failed!" << endl;
                return;
            }
        }
        if(dp != NULL)
            closedir(dp);
    
        ofstream outFile((path + file_name).c_str(), ios::out);
    
        if(outFile.good())
        {
            ostringstream convert;
            uInt index = 0;
            for(int i = 0; i < realRatings->size(); i ++)
            {
                convert.str("");
                convert << mIds.at(i) << ":" << endl;
                outFile << convert.str();
                for(std::vector<mRatings>::iterator it = realRatings->at(i)->begin();
                    it != realRatings->at(i)->end(); ++ it)
                {
                    convert.str("");
                    //convert << it->getId() << "," << it->getValue() << "," << predictedRatings->at(index) << endl;
                    convert << it->getId() << "," << predictedRatings->at(index) << endl;
                    outFile << convert.str();
                    index ++;
                }
                //outFile << convert.str();
            }
            outFile.close();
        }
    }
}