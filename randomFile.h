#ifndef RANDOMFILE_H
#define RANDOMFILE_H

#include <functional>
#include <fstream>
#include <vector>
#include <cstring>
#include <unordered_map>
#include <iomanip>
#include <iostream>
#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include "Rating.h"

#define RF_BLOCK_SIZE 4

using namespace std;

struct HashPair{
  template<class T1, class T2>
  size_t operator()(const pair<T1, T2>& p)const{
    auto hash1 = hash<T1>{}(p.first);
    auto hash2 = hash<T2>{}(p.second);
    return hash1 ^ hash2;
  }
};
typedef pair<uint64_t,uint64_t> IDS;
typedef pair<IDS,size_t> index_entry;

class RandomFile{

  size_t size;
  string random_file;
  string index_file;
  string info_file;
  mutex size_lock;

  void set_size(size_t new_size){
    ofstream outFile;
    outFile.open(info_file, ios::binary);
    outFile.seekp(0);
    outFile.write((char*)&(new_size), sizeof(size_t));
    outFile.close();
  }

public:
  unordered_map<IDS, size_t,HashPair> index;
  RandomFile()= default;
  RandomFile(string name_csv){
    string name = "randomFile";
    // inicializar los nombres
    this->random_file = name + ".bin";
    this->index_file = "index_" + name + ".bin";
    this->info_file = "info_" + name + ".bin";
    // made the files
    ofstream file;
    file.open(this->random_file, ios::binary);
    file.close();
    file.open(this->index_file, ios::binary);
    file.close();
    //poner el size = 0
    file.open(this->info_file, ios::binary);
    size_t aux = 0;
    this->size = aux;
    file.write((char*) &aux, sizeof(aux));
    file.close();
    update_csv(name_csv);
 }
  /*
   size_t get_size();
   void open(string name);
   void update_csv(QString file_name);
   void insert(Rating to_insert);
   void update_index();
   Rating search_openIndex(uint64_t user = 0, uint64_t movie = 0);
   Rating search_fixedIndex(uint64_t user = 0, uint64_t movie = 0);
   void read_index_from(size_t point);
   void update(Rating r);
   */
  size_t get_size(){
    ifstream inFile;
    size_t aux;
    inFile.open(info_file, ios::binary);
    inFile.seekg(0);
    inFile.read((char*)&(aux), sizeof(size_t));
    inFile.close();
    return aux;
  }

  void open(string name){
    //Crea un RandomFile
    fstream fs;
    this->random_file = name + ".bin";
    this->index_file = "index_" + name + ".bin";
    this->info_file = "info_" + name + ".bin";
    this->size = get_size();
  }

  void update_csv(string file_name){
      ifstream inFile;
      ofstream outFile, outIndex;
      //open the csv file
      
      inFile.open(file_name);
      outFile.open(this->random_file,ios::in | ios::binary | ios::app);
      outIndex.open(this->index_file,ios::in | ios::binary | ios::app);
      string aux;
      size_t num_ratings = 0;

    if(inFile.is_open()){
        getline(inFile,aux);
        float a,b,c,d;
        string str;
        index_entry idx_tmp;
        Rating tmp;
       while(getline(inFile,aux)){
           stringstream ss(aux);
          getline(ss,str,',');
          tmp.userId = stof(str);
          getline(ss,str,',');
          tmp.movieId = stof(str);
          getline(ss,str,',');
          tmp.rating = stof(str);
          getline(ss,str,',');
          tmp.timestamp = stof(str);
          //insert in the binary file
          //Rating tmp(a,b,c,d);
          outFile.write((char*) &tmp, sizeof(tmp));
          // aqui se debe insertar en el index
          idx_tmp.first.first = tmp.userId;
          idx_tmp.first.second = tmp.movieId;
          idx_tmp.second = num_ratings + get_size();
          outIndex.write((char*)&idx_tmp,sizeof(idx_tmp));

          ++num_ratings;
        }
        set_size(num_ratings + get_size());
        outFile.close();
        inFile.close();
        outIndex.close();
       }else cout << "Can't open the file "<<file_name <<endl;
  }


    void insert(Rating to_insert){
    //Write on data file
    ofstream outFile;
    int position;
    outFile.open(random_file, ios::in | ios::binary);
    if(outFile.is_open()){
        this->size_lock.lock();
        position = get_size();
        set_size(position+1);
        this->size_lock.unlock();
        outFile.seekp(position*sizeof(Rating));
      outFile.write((char*)& to_insert, sizeof(to_insert));
      outFile.close();
    }else{
      cout<<"Couldn't open the data file"<<endl;
      return;
    }

    //Write on index file
    index_entry entry;
    entry.first.first = to_insert.userId;
    entry.first.second = to_insert.movieId;
    entry.second = position;
    ofstream indexFile;
    indexFile.open(index_file, ios::in| ios::binary);
    if(indexFile.is_open()){
      indexFile.seekp(position*sizeof(entry));
      indexFile.write((char*)& entry, sizeof(entry));
      indexFile.close();
    }else{
      cout<<"Couldn't open the index file"<<endl;
      return;
    }
  }

  void update_index(){
    ifstream indexFile;
    indexFile.open(index_file, ios::binary);
    if(indexFile.is_open()){
      index_entry aux;

      while(indexFile.read((char*)&aux, sizeof(aux))){
        index.insert({aux.first,aux.second});
      }
      indexFile.close();
      return;
    }else{
      std::cout<<"Couldn't open the index file"<<std::endl;
      return;
    }
  }

  void read_index_from(size_t point){
    ifstream indexFile;
    indexFile.open(index_file, ios::binary);
    if(indexFile.is_open()){
      index_entry aux;
      indexFile.seekg(point*sizeof(index_entry)*RF_BLOCK_SIZE);
      for(size_t i = 0;i<RF_BLOCK_SIZE;i++){
        if(indexFile.read((char*)&aux, sizeof(aux))){
          index.insert({aux.first,aux.second});
        }else{break;}
      }
    }
  }

  Rating search_openIndex(uint64_t userid_s, uint64_t movieid_s){
    Rating ans;
    IDS pair_s(userid_s,movieid_s);
    size_t address;
    RandomFile::update_index();
    ifstream file;
    if(index.find(pair_s) == index.end()){
      std::cout<<"Not found"<<std::endl;
      return ans;
    }else{
      address = index.find(pair_s)->second;
      file.open(random_file, ios::binary);
      file.seekg(address*sizeof(ans));
      file.read((char*)&ans,sizeof(ans));
      file.close();
      std::cout<<"Found"<<std::endl;
      return ans;
    }
  }

  Rating search_fixedIndex(uint64_t userid_s, uint64_t movieid_s){
    Rating ans;
    IDS pair_s(userid_s,movieid_s);
    size_t address;
    ifstream file;
    for(size_t i = 0; i<=get_size()/RF_BLOCK_SIZE;i++){
        index.clear();
        read_index_from(i);
        auto result = index.find(pair_s);
        for(auto& i : index){
          cout<< i.first.first << "|" << i.first.second << "|" << i.second<<endl;
        }
        cout<<endl;
        if(result == index.end()){
          continue;
        }
        else{
          address = index.find(pair_s)->second;
          file.open(random_file, ios::binary);
          file.seekg(address*sizeof(ans));
          file.read((char*)&ans,sizeof(ans));
          file.close();
          std::cout<<"Found"<<std::endl;
          return ans;
        }
    }
    std::cout<<"Not found"<<std::endl;
    return ans;
  }

  void update(Rating r){
    IDS pair_s(r.userId,r.movieId);
    size_t address;
    ofstream file;
    for(size_t i = 0; i<=get_size()/RF_BLOCK_SIZE;i++){
        index.clear();  //limpia el index anterior
        read_index_from(i);
        if(index.find(pair_s) == index.end()){
          continue;
        }else{
          address = index.find(pair_s)->second;
          file.open(random_file, ios::binary | ios::in);
          file.seekp(address*sizeof(r));
          file.write((char*)&r,sizeof(r));
          file.close();
          std::cout<<"Updated"<<std::endl;
          break;
        }
    std::cout<<"Not found"<<std::endl;
    }
  }
 };




#endif // RANDOMFILE_H