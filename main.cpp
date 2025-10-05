/** SELLIER Themis 3A-AVM P2 C++*/

#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <vector>
#include <assert.h>
#include <fstream>

using namespace std;

/*les points RGB de l'image
Cette classe represente les points et leurs coordonnées RGB*/
class RGB
{
    public:
        double m_r, m_g, m_b;

        //Constructeurs
        RGB();//sans parametre
        RGB(double r, double g, double b); //clonage
        RGB& operator=(const RGB& rgb);//affectation
        RGB(const RGB& rgb);//clonage
        //Destructeur
        ~RGB() = default;

        //methodes
        double distance_(RGB rgb); // calcule la distance euclidienne entre deux points RGB
        void random_RGB(); // donne un RGB aleatoire
};

/* surface de dessin
contient la taille de l'image et les pixels
les pixels RGB sont mis a la suite dans un tableau de sorte que trois cases representent un seul pixel
la composante R,la composante G, la composante B*/
class Surface
{
    public:
        int m_w; // largeur
        int m_h; // hauteur
        vector<double> m_data; // valeurs RGB des pixels

        //Constructeurs
        Surface() = default;
        Surface(int w, int h, vector<double> data);
        Surface(int w, int h);
        Surface& operator= (const Surface& surf);
        //Destructeur
        ~Surface() = default;

        //methodes
        int ppm_write(ofstream& f); // ecrit le fichier de sortie f
        int ppm_read(ifstream& f); // lit les valeurs dans le fichier d'entree f
        void print_finalRGB(vector<RGB>& p, vector<RGB> m, int k, vector<int> a); // remplit les points RGB avec les couleurs des moyennes
        void copyRGB_in_data(vector<RGB> p); // copie les points RGB dans le tableau de la classe surface
};

/**Methodes appartenant aux classes : */

/** pour RGB */
// Constructeurs
RGB::RGB(): m_r(0), m_g(0), m_b(0) {};

RGB::RGB(double r, double g, double b): m_r(r), m_g(g), m_b(b){}

RGB& RGB::operator=(const RGB& rgb)
{
    m_r = rgb.m_r;
    m_g = rgb.m_g;
    m_b = rgb.m_b;
    return *this;
}

RGB::RGB(const RGB& rgb): m_r(rgb.m_r), m_g(rgb.m_g), m_b(rgb.m_b){}

//methodes
double RGB::distance_(RGB rgb)
{
    // calcule la distance euclidienne entre le point de la classe et un autre point donne en parametre

    double dr, dg, db; // on declare les variables
    //on les initialise en fonction des grandeurs des points de depart
    if(this->m_r < rgb.m_r){
            dr = rgb.m_r - this->m_r;
    }
    else{
            dr = this->m_r - rgb.m_r;
    }
    if(this->m_g < rgb.m_g){
            dg = rgb.m_g - this->m_g;
    }
    else{
            dg = this->m_g - rgb.m_g;
    }
    if(this->m_b < rgb.m_b){
            db = rgb.m_b - this->m_b;
    }
    else{
            db = this->m_b - rgb.m_b;
    }
    //on retourne la distance euclidienne
    return sqrt(dr*dr + dg*dg + db*db);
}

void RGB::random_RGB()
{
    // remplit RGB avec des valeurs aleatoires valides (comprises entre 0 et 1)

    random_device rd; // on prepare notre intervalle dans lequel on va piocher des valeurs
    mt19937 gen(rd());
    uniform_real_distribution<> distrib(0, 255); // on prend de 0 a 255 par rapport aux normes des RGB des pixels
    //on met les valeurs entre 0 et 1
    m_r = distrib(gen) / 255;
    m_g = distrib(gen) / 255;
    m_b = distrib(gen) / 255;
}

/** pour Surface */
// Constructeurs
Surface::Surface(int w, int h, vector<double> data)
{
    m_w = w;
    m_h = h;
    vector<double> m_data(data);
}

Surface::Surface(int w, int h)
{
    m_w = w;
    m_h = h;
    vector<double> m_data(w*h);
}

Surface& Surface::operator= (const Surface& surf)
{
    m_w = surf.m_w;
    m_h = surf.m_h;
    vector<double> m_data(surf.m_data);
    return *this;
}

//methodes

//Fonctions pour la gestion de fichier
int Surface::ppm_write(ofstream& f)
{
    //permet d ecrire le fichier de sortie avce les informations de la surface

    int maxi = 255;

    f << "P3\n#_png_write\n" << m_w << " " << m_h <<"\n" << maxi << "\n"; //on ecrit l entete du fichier
    for (int y = 0 ; y < m_h ; ++y)
    {
        for (int x = 0 ; x < m_w ; ++x)
        {
            int lineaire = (y * m_w + x) * 3 ; //on parcout le tableau de pixels
            /*
            on ecrit les valeurs RGB entre 0 et 255 dans le fichier de sortie
            on y ecrit les valeurs r, g et b du pixel a la position lineaire
            on saute des lignes apres chaque valeur ecrite pour correspondre au format de
             fichier qu'on souhaite en sortie et eviter les bugs
            */
            f << static_cast<int>(m_data[lineaire+0] * maxi) <<"\n";
            f << static_cast<int>(m_data[lineaire+1] * maxi) <<"\n";
            f << static_cast<int>(m_data[lineaire+2] * maxi) <<"\n";
        }
    }
    return 1;
}


int Surface::ppm_read(ifstream& f)
{
    /* lit, vers une surface, une image au format PPM sur un flot d'entree
    retourne 1 si la lecture s'est bien terminee
    0 sinon
    */

    // on verifie que l entete est ecrite correctement
    if (f.get() != 'P')
        return 0;
    if (f.get() != '3')
        return 0;
    if (f.get() != '\n')
        return 0;

    char c;
    // on passe la ligne que GIMP ecrit et qui ne donne pas de donnees importantes
    while ((c = f.get()) == '#')
        while (f.get() != '\n')
            ;
    f.unget();

    //on commence la recuperation de donnees
    //d'abord la largeur, hauteur de l'image et le maximum
    int width, height, maxi;
    //on fait la lecture et on verifie en meme temps que tout se passe correctement
    if (!(f >> width >> height >> maxi))
        return 0;

    //on initialise la surface qui contiendra les donnees finales
    Surface surf{width, height};
    //on la lie a la surface qu'on est en train d'utiliser au depart
    *this = surf;

    //on recupere toutes les donnees jusqu'a avoir parcouru toutes l image
    for (int y = 0 ; y < m_h ; ++y)
        {
            for (int x = 0 ; x < m_w ; ++x)
            {
                //on lit les couleurs des pixels RGB
                int value_red, value_green, value_blue;
                if (!(f >> value_red >> value_green >> value_blue))
                    return 0;
                // on les assigne au tableau de donnees
                m_data.push_back(static_cast<double>(value_red) / maxi);
                m_data.push_back(static_cast<double>(value_green) / maxi);
                m_data.push_back(static_cast<double>(value_blue) / maxi);
            }
        }
    //la lecture s est bien passee, on retourne donc 1
    return 1;
}

void Surface::print_finalRGB(vector<RGB>& p, vector<RGB> m, int k, vector<int> a)
{
    //on assigne aux points les couleurs des clusters (moyennes) auquels ils appartiennent

    for(int i = 0; i < m_w * m_h; ++i) // on parcourt les points
    {
        for(int j = 0; j < k; ++j) //on parcourt les clusters (moyennes)
        {
            // on verifie que le point appartient au cluster j
            if(a[i] == j){
                p[i].m_r = m[j].m_r;
                p[i].m_g = m[j].m_g;
                p[i].m_b = m[j].m_b;
            }
        }
    }
}

void Surface::copyRGB_in_data(vector<RGB> p)
{
    // on transpose les donnees RGB dans le tableau data de la surface

    // on parcourt les vecteurs
    for (int y = 0 ; y < m_h ; ++y)
    {
        for (int x = 0 ; x < m_w ; ++x)
        {
            int lineaire = (y * m_w + x) * 3 ;
            // on assigne les bonnes valeurs aux bonnes cases
            m_data[lineaire + 0] = p[y * m_w + x].m_r ;
            m_data[lineaire + 1] = p[y * m_w + x].m_g ;
            m_data[lineaire + 2] = p[y * m_w + x].m_b ;
        }
    }
}

/**Methodes pour toutes les classes*/
RGB moyenne_(vector<RGB> p, vector<int> a, int cluster)
{
    //calcule la moyenne RGB des points appartenant au cluster

    //on declare la moyenne
    RGB moyenne;
    //on prepare les variables
    int n = p.size();
    double somme_r = 0;
    double somme_g = 0;
    double somme_b = 0;
    int total = 0;

    //on parcourt les points
    for (int i = 0; i < n; ++i)
    {
        if(a[i] == cluster) //on verifie qu'ils appartiennent au cluster voulu
        {
            somme_r = somme_r + p[i].m_r;
            somme_g = somme_g + p[i].m_g;
            somme_b = somme_b + p[i].m_b;
            total = total + 1;//le nombre de valeur pour la moyenne
        }
    }
    //ensuite on fait la nouvelle moyenne pour le barycentre
    if(total!=0){
        moyenne.m_r = (somme_r / total);
        moyenne.m_g = (somme_g / total);
        moyenne.m_b = (somme_b / total);
    }
    else{
        //si aucun point n appartient au cluster, on genere une nouvelle moyenne pour posseder, a terme,
        // des clusters non vides
        moyenne.random_RGB();
    }

    //on retourne la moyenne
    return moyenne;
}

bool cluster_(vector<RGB> p, vector<RGB>& m, int k, vector<int> a)//calcule l'ensemble des moyennes pour les nouveaux barycentres et retourne true si tous les barycentres sont ideaux
{
    //calcule l'ensemble des moyennes pour les nouveaux barycentres et retourne true si tous
    // les barycentres sont ideaux, false sinon

    //on prepare les variables
    bool barycentre_ideal = false;
    int nb_ideal = 0;
    // on parcourt les moyennes
    for(int i = 0; i < k; ++i)
    {
        RGB c = moyenne_(p, a, i);
        //si les RGB sont egaux, le barycentre est ideal
        if(m[i].m_r == c.m_r && m[i].m_g == c.m_g && m[i].m_b == c.m_b){
                nb_ideal += 1;
        }
        m[i] = c;
    }
    if (nb_ideal == k){
            barycentre_ideal = true;
    }

    return barycentre_ideal;
}

void barycentre_(vector<RGB> p, vector<RGB> m, int k, vector<int>& a)
{
    // assigne les points au barycentre le plus proche

    //on parcourt les points
    for (int i = 0; i < p.size() ; ++i)
    {
        // on prepare les variables
        int rang = 0;
        double minimum = p[i].distance_(m[0]);
        //on parcourt les moyennes
        for(int j = 0; j < k; j++)
        {
            //on cherche la moyenne la plus proche
            if(p[i].distance_(m[j]) < minimum)
            {
                minimum = p[i].distance_(m[j]);//on a le barycentre le plus proche pour le point i
                rang = j;
            }
        }
        // on assigne le point a la moyenne
        a[i] = rang; //on attribut à chaque point son cluster de point le plus proche (qui depend du nombre de moyenne)
    }
}

int main()
{
    // creation d'une image originale
    Surface surf;

    //lecture du fichier
    ifstream f("image.ppm");
    //recuperation des donnees dans la surface surf
    int ok = surf.ppm_read(f);
    //controle sanitaire
    if(!ok){
        cout << "Erreur a la lecture du fichier";
        return 1;
    }
    //on ferme le fichier apres avoir recupere les donnees
    f.close();

    //on prepare les variables pour la suite :

    //on recupere les datas de surface pour les mettre dans un vecteur RGB
    vector<RGB> p;
    for (auto i = surf.m_data.begin(); i < surf.m_data.end(); i = i + 3)
    {
        RGB temp{*i, *(i+1), *(i+2)};
        p.push_back(temp);
    }

    //on cree les moyennes initiales
    vector<RGB> m;
    int k;
    //on demande a l utilisateur de choisir le nombre de couleurs qu'il souhaite sur l'image finale
    cout << "Rentrer le nombre de moyennes: ";
    cin >> k;
    //on assigne des moyennes aleatoires
    for (int i = 0; i < k ; ++i)
    {
        RGB q{};
        q.random_RGB();
        m.push_back(q);
    }

    //on declare le tableau qui contient les numeros de clusters auquels les points appartiennent
    vector<int> a(p.size());

    //on initialise le barycentre
    barycentre_(p, m, k, a);

    // on calcule les nouvelles moyennes jusqu'a ce que les barycentres soient ideaux
    bool bar_ideal = false;
    do
    {
        bar_ideal = cluster_(p, m, k, a);
        barycentre_(p, m, k, a);

    }while(bar_ideal == false);

    // on change les couleurs des points RGB
    surf.print_finalRGB(p, m, k, a);
    // on entre dans les donnees de la surface les nouvelles valeurs des points
    surf.copyRGB_in_data(p);

    // on ouvre le fichier de sortie
    ofstream output("draw.ppm");
    // controle sanitaire
    if (!output.is_open())
    {
        cout << "Erreur d'ouverture du fichier de sortie" << endl;
        return 1;
    }

    // on ecrit les donnees finales dans le fichier de sortie
    surf.ppm_write(output);
    //on ferme le fichier de sortie
    output.close();

    return 0;
}
