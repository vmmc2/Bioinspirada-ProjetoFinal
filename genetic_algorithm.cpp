#include <bits/stdc++.h>

using namespace std;

class Photo{
    public:
        int photo_id;
        set<string> photo_tags;

        Photo(int id, vector<string> tags){
            this->photo_id = id;
            for(auto tag : tags){
                this->photo_tags.insert(tag);
            }
        }
};

class Slide{
    public:
        vector<Photo> photos;
        set<string> tags;

        Slide(vector<Photo> slide_photos){
            this->photos = slide_photos;
            this->tags = this->get_tags();
        }

        set<string> get_tags(){
            set<string> tags;

            for(auto photo : this->photos){
                for(auto tag : photo.photo_tags){
                    tags.insert(tag);
                }
            }

            return tags;
        }
};

class SlideShow{
    public:
        int fitness;
        vector<Slide> slides;
        string mutation_type;
        double mutation_probability;

        SlideShow(vector<Slide> slides, string mutation_type, double mutation_probability){
            this->fitness = this->get_fitness();
            this->slides = slides;
            this->mutation_type = mutation_type;
            this->mutation_probability = mutation_probability;
        }

        bool operator < (const SlideShow& s2){
            if(this->fitness < s2.fitness) return true;
            else return false;
        }

        bool operator <= (const SlideShow& s2){
            if(this->fitness <= s2.fitness) return true;
            else return false;
        }

        bool operator == (const SlideShow& s2){
            if(this->fitness == s2.fitness) return true;
            else return false;
        }

        bool operator > (const SlideShow& s2){
            if(this->fitness > s2.fitness) return true;
            else return false;
        }

        bool operator >= (const SlideShow& s2){
            if(this->fitness >= s2.fitness) return true;
            else return false;
        }

        int get_fitness(){
            int fitness = 0;
            int num_slides = (int)this->slides.size();

            for(int i = 0; i < num_slides - 1; i++){
                set<string> fst_slides_tags = this->slides[i].tags;
                set<string> snd_slides_tags = this->slides[i + 1].tags;

                set<string> intersection_set_tags = this->get_set_intersection(fst_slides_tags, snd_slides_tags);
                set<string> only_fst_set_tags = this->get_set_difference(fst_slides_tags, snd_slides_tags);
                set<string> only_snd_set_tags = this->get_set_difference(snd_slides_tags, fst_slides_tags);

                int interest_factor = min({(int)intersection_set_tags.size(), (int)only_fst_set_tags.size(), (int)only_snd_set_tags.size()});
                fitness += interest_factor;
            }

            return fitness;
        }

        set<string> get_set_intersection(set<string> set1, set<string> set2){
            set<string> set3;

            for(auto tag1 : set1){
                if(set2.find(tag1) != set2.end()){
                    set3.insert(tag1);
                }
            }

            return set3;
        }

        set<string> get_set_difference(set<string> set1, set<string> set2){
            set<string> set3;

            for(auto tag1 : set1){
                if(set2.find(tag1) == set2.end()){
                    set3.insert(tag1);
                }
            }

            return set3;
        }

        void generic_mutation(){
            //cout << "THIS MUTATION TYPE: " << this->mutation_type << endl;
            if(this->mutation_type == "mutation_by_swap"){
                this->mutation_by_swap();
            }else if(this->mutation_type == "mutation_by_inversion"){
                this->mutation_by_inversion();
            }else if(this->mutation_type == "mutation_by_disturb"){
                this->mutation_by_disturb();
            }else{
                cout << "[ERROR]: INVALID MUTATION TYPE" << endl;
                exit(1);
            }
            
            return;
        }

        void mutation_by_swap(){
            srand(time(0));

            if(rand()/RAND_MAX <= this->mutation_probability){
                int index1 = rand() % (int)this->slides.size();
                int index2 = rand() % (int)this->slides.size();

                swap(this->slides[index1], this->slides[index2]);

                this->fitness = this->get_fitness();
            }
            
            //this->mutation_by_swap_vertical_photos();

            return;
        }

        void mutation_by_inversion(){
            srand(time(0));

            if(rand()/RAND_MAX <= this->mutation_probability){
                int index1 = rand() % (int)this->slides.size();
                int index2 = rand() % (int)this->slides.size();

                int left_index = min(index1, index2);
                int right_index = max(index1, index2);

                while(left_index < right_index){
                    swap(this->slides[left_index], this->slides[right_index]);
                    left_index++;
                    right_index--;
                }

                this->fitness = this->get_fitness();
            }

            return;
        }

        void mutation_by_disturb(){
            srand(time(0));

            if(rand()/RAND_MAX <= this->mutation_probability){
                unsigned int seed = chrono::system_clock::now().time_since_epoch().count();

                shuffle(this->slides.begin(), this->slides.end(), default_random_engine(seed));

                this->fitness = this->get_fitness();
            }

            return;
        }

        void mutation_by_swap_vertical_photos(){
            srand(time(0));
            
            unsigned int seed = chrono::system_clock::now().time_since_epoch().count();

            vector<int> indexes_of_vertical_slides;

            for(int i = 0; i < (int)this->slides.size(); i++){
                if(this->slides[i].photos.size() == 2){
                    indexes_of_vertical_slides.push_back(i);
                }
            }

            shuffle(indexes_of_vertical_slides.begin(), indexes_of_vertical_slides.end(), default_random_engine(seed));

            int index1 = indexes_of_vertical_slides[0];
            int index2 = indexes_of_vertical_slides[1];

            Photo photo_from_slide_1 = this->slides[index1].photos[0];
            Photo photo_from_slide_2 = this->slides[index2].photos[1];

            this->slides[index1].photos[0] = photo_from_slide_2;
            this->slides[index2].photos[1] = photo_from_slide_1;

            return;
        }
};

class GeneticAlgorithm{
    public:
        int num_executions;
        int num_iterations;
        int population_size;
        string mutation_type;
        double mutation_probability;
        double num_parents_percentage;
        vector<SlideShow> population;
        string current_test_case_filename;
        vector<string> testcases_filenames{"a_example.txt", "b_lovely_landscapes.txt", "c_memorable_moments.txt", "d_pet_pictures.txt", "e_shiny_selfies.txt"};


        GeneticAlgorithm(int num_executions, int num_iterations, int population_size, string mutation_type, double mutation_probability, double num_parents_percentage, string current_test_case_filename){
            this->num_executions = num_executions;
            this->num_iterations = num_iterations;
            this->population_size = population_size;
            this->mutation_type = mutation_type;
            this->mutation_probability = mutation_probability;
            this->num_parents_percentage = num_parents_percentage;
            this->current_test_case_filename = current_test_case_filename;
        }

        void initialize_population(){
            ifstream read(this->current_test_case_filename);

            int num_photos;
            
            read >> num_photos;

            int curr_id = 0;
            vector<Photo> vertical_photos;
            vector<Photo> horizontal_photos;

            for(int i = 0; i < num_photos; i++){
                string photo_type;
                int num_tags;
                vector<string> tags;

                read >> photo_type;
                read >> num_tags;

                for(int j = 0; j < num_tags; j++){
                    string curr_tag;
                    read >> curr_tag;
                    tags.push_back(curr_tag);
                }

                if(photo_type == "H"){
                    horizontal_photos.push_back(Photo(curr_id, tags));
                }else if(photo_type == "V"){
                    vertical_photos.push_back(Photo(curr_id, tags));
                }
                curr_id += 1;
            }


            for(int i = 0; i < this->population_size; i++){
                vector<Slide> all_slides;
                vector<Slide> vertical_photos_slides;
                vector<Slide> horizontal_photos_slides;

                unsigned int seed = chrono::system_clock::now().time_since_epoch().count();

                shuffle(vertical_photos.begin(), vertical_photos.end(), default_random_engine(seed));
                shuffle(horizontal_photos.begin(), horizontal_photos.end(), default_random_engine(seed));
                
                for(int j = 0; j < vertical_photos.size(); j += 2){
                    vertical_photos_slides.push_back(Slide(vector<Photo>{vertical_photos[j], vertical_photos[j + 1]}));
                }
                for(int j = 0; j < horizontal_photos.size(); j += 1){
                    horizontal_photos_slides.push_back(Slide(vector<Photo>{horizontal_photos[j]}));
                }

                for(int j = 0; j < vertical_photos_slides.size(); j++){
                    all_slides.push_back(vertical_photos_slides[j]);
                }
                for(int j = 0; j < horizontal_photos_slides.size(); j++){
                    all_slides.push_back(horizontal_photos_slides[j]);
                }

                shuffle(all_slides.begin(), all_slides.end(), default_random_engine(seed));

                this->population.push_back(SlideShow(all_slides, this->mutation_type, this->mutation_probability));

            }

            return;
        }

        void select_and_mutate_parents(){
            int num_parents = floor(this->num_parents_percentage * this->population_size);
            sort(this->population.begin(), this->population.end());

            for(int i = 0; i < num_parents; i++){
                SlideShow parent = this->population[i];
                parent.generic_mutation();
                this->population.push_back(parent);
            }
            return;
        }

        void select_survivors(){
            int counter = 0;
            int cursor = this->population.size() - 1;
            vector<SlideShow> aux;

            sort(this->population.begin(), this->population.end());
            while(counter < this->population_size){
                aux.push_back(this->population[cursor]);
                cursor--;
                counter++;
            }
            
            this->population = aux;

            return;
        }

        void benchmark(){
            for(int i = 0; i < this->num_executions; i++){
                this->initialize_population();
                for(int j = 0; j < this->num_iterations; j++){
                    cout << "Running iteration " << (j + 1) << "..." << endl;
                    this->select_and_mutate_parents();
                    this->select_survivors();

                    double curr_iteration_mean_fitness = 0;
                    double curr_iteration_max_fitness = 0;

                    for(int k = 0; k < (int)this->population.size(); k++){
                        curr_iteration_mean_fitness += (double)this->population[k].fitness;
                        curr_iteration_max_fitness = max(curr_iteration_max_fitness, (double)this->population[k].fitness);
                    }
                    curr_iteration_mean_fitness /= (double)this->population.size();

                    cout << "Iteration Mean Fitness: " << curr_iteration_mean_fitness << endl;
                    cout << "Iteration Max Fitness: " << curr_iteration_max_fitness << endl;
                }
            }

            sort(this->population.begin(), this->population.end());
            SlideShow best_solution = this->population[this->population_size - 1];

            ofstream write("submission.txt");

            write << (int)best_solution.slides.size() << endl;

            for(int i = 0; i < (int)best_solution.slides.size(); i++){
                if(best_solution.slides[i].photos.size() == 1){
                    write << best_solution.slides[i].photos[0].photo_id << endl;
                }else if(best_solution.slides[i].photos.size() == 2){
                    write << best_solution.slides[i].photos[0].photo_id << " " << best_solution.slides[i].photos[1].photo_id << endl;
                }
            }

            write.close();

            return;
        }

        void testcases_statistics(){
            map<string,set<string>> num_tags_per_testcase;
            
            for(auto test_case_filename : this->testcases_filenames){
                ifstream read(test_case_filename);

                int num_photos;
                int num_vertical_photos = 0;
                int num_horizontal_photos = 0;

                read >> num_photos;

                for(int i = 0; i < num_photos; i++){
                    string photo_type;
                    int num_tags;

                    read >> photo_type;

                    if(photo_type == "H"){
                        num_horizontal_photos++;
                    }else if(photo_type == "V"){
                        num_vertical_photos++;
                    }

                    read >> num_tags;

                    for(int j = 0; j < num_tags; j++){
                        string curr_tag;
                        read >> curr_tag;
                        num_tags_per_testcase[test_case_filename].insert(curr_tag);
                    }
                }

                cout << "-------------------------------------------------------------------------------" << endl;
                cout << "Statistics of Test Case: " << test_case_filename << endl;
                cout << "Total Number of Vertical Photos: " << num_vertical_photos << endl;
                cout << "Total Number of Horizontal Photos: " << num_horizontal_photos << endl;
                cout << "Total Number of Tags: " << (int)num_tags_per_testcase[test_case_filename].size() << endl;
                cout << "-------------------------------------------------------------------------------" << endl;

            }

            return;
        }
};

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(0);

    GeneticAlgorithm genetic_algorithm(1, 150, 5, "mutation_by_inversion", 0.8, 1.0, "b_lovely_landscapes.txt");
    
    genetic_algorithm.testcases_statistics();
    
    genetic_algorithm.benchmark();

    return 0;
}
