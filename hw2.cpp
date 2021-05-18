#define _GLIBCXX_USE_CXX11_ABI 0/1
#include<iostream>
#include<string>
#include<vector>
#include<cstring>
#include<map>
#include<cstdlib>
#include<algorithm>

using namespace std;

#define GENERATION_SIZE 100
#define POPULATION_SIZE 100

void fitness(const vector<vector<int>> &clusters, map< int, const vector<int> > &m, int num_clusters){
    //clusters는 클러스터 번호마다 인덱스가 포함되어 있다.
    //m을 인자로 받아서 
    
    int cut_size = 0;

    for(int i = 0; i < clusters.size(); i++){
        for(int j = i; j < clusters.size(); j++){
            //clusters[i]
            //clusters[j]
        }
    }

}

void evalutate(const vector< vector<int> > &chromosomes, vector<int> &fitness_list, map< int, const vector<int> > &m, int num_clusters){

    for(int i = 0; i < chromosomes.size(); i++){ //100개의 염색체 각각에 대해
        vector<vector<int>> clusters;

        for(int j = 0; j < num_clusters; j++){
            vector<int> cluster;
            clusters.push_back(cluster);
        }
        
        for(int j = 0; j < chromosomes[i].size(); j++){ //염색체 길이 만큼 돌면서 각 클러스터에 인덱스를 추가함
            //chromosomes[i] == 010101001 ...
            clusters[chromosomes[i][j]].push_back(j);
        }

        fitness_list.push_back(fitness(clusters, map< int, const vector<int> > &m, int num_clusters));
    }
}

void initialize(vector< vector<int> > &chromosomes, int num_clusters, int num_nodes){
    //clusters : balanced 클러스터가 되도록 할 것
    //Max cut 일 경우엔 balanced 고려하지 않아도 됨.
    //count : 노드 개수
    for(int pop = 0 ; pop < POPULATION_SIZE; pop++){
        vector<int> chromosome;
        vector<int> cluster_count(num_clusters);
        
        int limit;
        int limit_count = 0;
        bool divided = false;

        if ( num_nodes % num_clusters == 0){
            limit = int(num_nodes / num_clusters);
        }
        else{
            limit = int(num_nodes / num_clusters)+1;
        }        
        //for문 돌면서 각 노드마다 클러스터를 할당
        for(int i = 0; i < num_nodes; i++){               
            int rand_num = rand() % num_clusters;                                   
            while(cluster_count[rand_num] >= limit){                
                rand_num = rand() % num_clusters;              
            }
            
            cluster_count[rand_num]++;            
            
            chromosome.push_back(rand_num);

            if(divided){                
                continue;
            }

            if (cluster_count[rand_num] == limit){                
                limit_count++;                                                
                if (limit_count == num_nodes - int(num_nodes / num_clusters)*num_clusters){                                                                               
                    limit--;
                    divided = true;
                }
            }
            
        }        
        chromosomes.push_back(chromosome);
    }
}

vector<string> split(string s, string divid) {
    //divid에 맞게 문자열 파싱한 벡터 반환
	vector<string> v;
	char* c = strtok((char*)s.c_str(), divid.c_str());
	while (c) {
		v.push_back(c);
		c = strtok(NULL, divid.c_str());
	}
	return v;
}

int main(int argc, char *argv[]){
    string s;    
    int count = 0;    
    //입력 파일에 대해 그래프 노드 추가
    map< int, vector<int> > m;
    while(getline(cin, s)){
        vector<string> n = split(s, " ");
        if(n.size() > 3){ //추가할 노드가 있다면
            for(int j = 3; j < n.size() ; j++){
                   m[count].push_back(stoi(n[j])-1);
            }
        }
        count++;
    }

    // 인코딩
    //K개로 클러스터링 된 클러스터와, 노드 개수 전달해서 초기화
    vector<vector<int>> chromosomes;    
    vector<int> fitness_list;

    //chromosome, m 에는 0~499 노드 번호에 대한 정보 담겨있음
    initialize(chromosomes, atoi(argv[1]), count);
    evalutate(chromosomes, fitness_list, m, atoi(argv[1]));

    // for(int n_gen = 0; n_gen < GENERATION_SIZE; n_gen++){


    //     //cross

    //     //평가하고
    // }

    //연산 정의
}



// for(int i = 0; i < chromosomes.size(); i++){
    //     for(int j = 0; j<chromosomes[i].size(); j++){
    //         cout << chromosomes[i][j] << " ";
    //     }
    //     cout << endl;        
    // }