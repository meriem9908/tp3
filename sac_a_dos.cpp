#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define N 100

// définir la structure arbre
  typedef struct noed{ 
                int    num_sommet ;  // le numéro du sommet 0 , 1 , 2 ...
  	  int    borne_superieur;  // la valeur du noeud 
  	  float  valeurs_variables[N];  // valeur des variable objets [0,1] 
  	  int    fixer_variables[N]; // fixé lobjet 
  	                             // 0 : fixé a zéro   1 : fixé a 1
  	                             // -1 : pas encor fixé
	  struct noed*   gauche;  // la partie gauche		
	  struct noed*   droite;  // la partie droite
	  
	}arborescence ; 
typedef arborescence*   arbre ;

// les variable globale
int     nb_variable ; // nb variable
float   coeff_fon_objective[N];  // vecteur objective
float   coeff_contrainte[N];  // vecteur de  contrainte
float   tab_efficacite[N];  // vecteur éfficacité des objet (trier)
float   tab_indice[N]; // vecteur des objet trier
int     borne_inferieur;  // la borne inférieur
int     resultat_final[N] ; // vecteur des objet (solution finale)
arbre   arbre_resultat ;  // l arbre qui contient tous les sous probleme
int     choix ; // recomancer / quiter
int     voir_detaille  ; // voir les détaille ou pas
int     num_som = 0 ; // le numéro du sommet commance par 0

//prototype :
void affiche_result_node(arbre node);
arbre init_arbre();


// les fonctions :

int is_entier(float n){ // tester si un nombre est entier (dans se cas entre 0 et 1)
	int     parti_entiere ;
	float   parti_non_entiere ;
	parti_entiere       = int(n);
	
	parti_non_entiere   = float(n) - parti_entiere ;
	
	if((parti_non_entiere <= 0.001  && parti_non_entiere >= -0.001 ) ||
	   (parti_non_entiere < 1  && parti_non_entiere >= 0.99 ) ||
	   (parti_non_entiere <= -0.99  && parti_non_entiere > -1 ) 
	  ){
	      return 1;	
	}
  return 0;
}


int solution_non_entiere(arbre a){  // tester si le noeud contient une solution int
	int i ;
	
	for(i=1 ; i<= nb_variable ; i++){
		if(!is_entier(a->valeurs_variables[i]) ){
		     //indice_variable_non_int = i;
		     return i; // il existe une valeur non entier	
		}
	}
	
	return 0; // toutes les valeur sont de type int (0 / 1)
}




void lire_nb_variable(){  // lire le nombre des variable
	char  chaine[N];
	do{
	    printf("  Donner le nombre des variable :");
	    scanf("%s",chaine);
	    nb_variable = atoi(chaine);
	    
	}while(nb_variable <= 0);
	
	
}






void lire_fonction_objective(){  // lire la fonction objective
     int  i;
     printf("Donner la fonction objective : ");
     for(i=1 ; i<= nb_variable ; i++){
          	
          printf("\n     X%d =  ",i);
          scanf("%f",&coeff_fon_objective[i]);
          	
     }	
	
}




void lire_contrainte(){  // lire la contrainte
     int  i;
     printf("Donner la contrainte : ");
          // lire le poids max
               printf("\n     Donner le poids Max :",i);
          scanf("%f",&coeff_contrainte[nb_variable+1]);
          
     for(i=1 ; i<= nb_variable ; i++){
          do{
               printf("\n     X%d =  ",i);
               scanf("%f",&coeff_contrainte[i]); 
	 if(coeff_contrainte[i] > coeff_contrainte[nb_variable+1]){
	     printf("            X%d doit etre superieur ou egale a %.2f  \n ",i,coeff_contrainte[nb_variable+1]);
	 }         	
          }while(coeff_contrainte[i] > coeff_contrainte[nb_variable+1]);	

          	
     }	

               	
}




void permuter(float *a , float *b){   // la permutation
	float  var_temporaire ;
	var_temporaire =  *a;
	*a   =   *b ;
	*b   =   var_temporaire ;
	
}



void initialiser_efficacite(){  // calculer l efficacité
	int i ;
	for(i=1 ; i<= nb_variable ; i++){
	     tab_efficacite[i] = coeff_fon_objective[i] / coeff_contrainte[i];
	}
	
}




void initialiser_indices(){  // initialiser les indice des variable
	int i ;
	for(i=1 ; i<= nb_variable ; i++){
	    tab_indice[i] = i ;
	}
		
}


void initialise_var_fixe(){ // tous les variable n ont pas été fixé
	int  i;
	for(i=1 ; i<= nb_variable ; i++){
		// pas encor fixé
		arbre_resultat->fixer_variables[i] = -1; 
	}
}


void tri_efficacite(){	// trier les efficacite par ordre décroissant
	int  i,j;
	

	for(i=1 ; i<= nb_variable ; i++){
		for(j= i+1 ;j<= nb_variable ; j++){
		   if(tab_efficacite[i] < tab_efficacite[j]){
		     // trier  lefficacité
		     permuter(&tab_efficacite[i],&tab_efficacite[j]);
		     // triér  l objet
		     permuter(&tab_indice[i],&tab_indice[j]);
	                 }
		}
	}

	
}

void calcul_sol_realisable(){  // calculer la premiere solution réalisable
	float   volume_max ;
	int     i , indice;
	// trouver les valeur des objet (1 / 0 )
	volume_max = coeff_contrainte[nb_variable+1];
	
	for(i=1 ; i<= nb_variable ; i++){
	       indice = tab_indice[i];
	       if(coeff_contrainte[indice] <= volume_max){
	           volume_max = volume_max - coeff_contrainte[indice] ;
	           resultat_final[indice]  = 1; // on peut prendre l objet   	
	       }else{
	           resultat_final[indice]  = 0; // on peut pas le prendre	
	       }	
	}
	
	// calculer la borne inférieur
	borne_inferieur =  0 ;
	for(i=1 ; i<= nb_variable ;i++){
        	   borne_inferieur = borne_inferieur  +
	           ( coeff_fon_objective[i] * resultat_final[i]); 			
	}
}

float take_object(arbre le_node){
	int i , indice ;
	float vol_max = coeff_contrainte[nb_variable + 1];
	
	for(i=1 ;i<=nb_variable ; i++){
	     indice = tab_indice[i];
	     if(le_node->fixer_variables[indice] == 1){
	          vol_max = vol_max - coeff_contrainte[indice];
	     }	
	}	
	return vol_max ;	
}

void remplire_valeur(arbre le_node){
	int i;
	for(i=1 ; i<= nb_variable ; i++){
		
		if(le_node->fixer_variables[i] == 1){
			le_node->valeurs_variables[i] = 1 ;
			
		}else{
			le_node->valeurs_variables[i] = 0 ;	
		} 
		
	}
}

void iliminate_object(arbre le_node,float volume_max){
	int i;
	int indice;
	for(i=1 ; i<= nb_variable ; i++){
		indice = tab_indice[i];	
		if(le_node->fixer_variables[indice] == -1){
			if(coeff_contrainte[indice] > volume_max){
			   le_node->fixer_variables[indice] = 0;		
			}	
		}
	}
}
void calcul_borne_superieur(arbre le_node ){ // la borne supérieur du sommet S0

	float   volume_max ;
	int     i , indice;

	//prendre les objet fixé a 1
	volume_max = take_object(le_node);
	
	//iliminer les objet qu on peut pas les prendre
	iliminate_object(le_node,volume_max);
	
	// remplire les valeur des variable du node
	remplire_valeur(le_node); 


	
	// trouver les valeur de X
	for(i=1 ; i<= nb_variable ; i++){
		
	       indice = tab_indice[i];
	       
	       if(le_node->fixer_variables[indice] == -1){
	       	if(coeff_contrainte[indice] <= volume_max){
		    volume_max = volume_max - coeff_contrainte[indice] ;
	                  le_node->valeurs_variables[indice]  = 1;	       		
		}else{
	            		le_node->valeurs_variables[indice]=
	              	 volume_max  / coeff_contrainte[indice];
	            		break ;	
	      	 }	
	       }
	       if(le_node->fixer_variables[indice] == 0){
	       	le_node->valeurs_variables[indice]  = 0;	       	       	  	
	      }	

	}	
	
	
	// calculer la borne superieur
	// arbre_resultat->borne_superieur = 0 ;// bahnes : a supprimer
	
    le_node->borne_superieur=0;/// ajoutée par bahnes
	for(i=1 ; i<= nb_variable ;i++){
        	   le_node->borne_superieur = le_node->borne_superieur  +
	           ( coeff_fon_objective[i] * le_node->valeurs_variables[i]); 
              		
	}


}

void change_result(arbre a){ 
	int  i;
	// changer le résultat si on obtient une nouvelle solution optimale
	for(i=1; i<= nb_variable ; i++){
		resultat_final[i] = a->valeurs_variables[i];
	}
}





 
     // tester si larbre est vide
int est_vide(arbre a){
	if(a == NULL)
	   return  1;
	return 0;   
}


void fixe_les_indice(arbre fils_gauche,arbre le_node,int fixe_or_no ,int indice){
	int i;
	for(i=1 ; i<= nb_variable ; i++){
		fils_gauche->fixer_variables[i] = le_node->fixer_variables[i];
	}
	fils_gauche->fixer_variables[indice] = fixe_or_no ;

}

void separation_evaluation(arbre le_node){  // construire les sommets
	
	int  i;
	int  indice_variable_non_int;

	
	// calculer la borne supérier		
	calcul_borne_superieur(le_node);
	
	// le numéro du sommet
	le_node->num_sommet = num_som ;
	num_som++;

	if(voir_detaille){
	  // afficher le résultat de ce node
	  affiche_result_node(le_node);		
	}
	
	
	// séparer le node si nécéssaire
	indice_variable_non_int = solution_non_entiere(le_node);
	
	if(indice_variable_non_int){
	     if(le_node->borne_superieur > borne_inferieur){
	     	
	     	// créer un fils gauche et fixé la variable par 0
	     	arbre fils_gauche = init_arbre() ;
       		fils_gauche=(arbre)malloc(sizeof(arborescence));
	     	fixe_les_indice(fils_gauche,le_node,0,indice_variable_non_int);
	     	le_node->gauche = fils_gauche ;
	     	
	     	
	     	// créer un fils droite et fixé la variable par 1
	     	arbre fils_droite = init_arbre() ;
       		fils_droite =(arbre)malloc(sizeof(arborescence));
       		fixe_les_indice(fils_droite,le_node,1,indice_variable_non_int);
		le_node->droite = fils_droite ; 
		
		// appliquer la séparation :
		separation_evaluation(fils_gauche);
		separation_evaluation(fils_droite);    		
       		
	     }else{
	     	// arbre point ver NULL (pas de fils)	
		le_node->droite =   NULL ;
		le_node->gauche =   NULL ;
	     }	
		
	}else{
	     if(le_node->borne_superieur > borne_inferieur){
	         	// alor on change la borne inferieur
    	   	borne_inferieur = le_node->borne_superieur;
    	   	// et on change le résultat
    	   	change_result(le_node);	
	     }
		// arbre point ver NULL (pas de fils)	
		le_node->droite =   NULL ;
		le_node->gauche =   NULL ;		    
		
	}


}


arbre init_arbre(){ // initialiser l arbre
	return NULL ;
}


//   l affichage :  ***************************************************
void affiche_fonction_objective(){
	int   i ;
	i  =  1;
	printf("        La fonction objective :\n");
	printf("              MaxZ =  %.2fX%d", coeff_fon_objective[i],i);
	for(i=2 ; i<= nb_variable ; i++){
	     printf("  +  %.2fX%d", coeff_fon_objective[i],i);	
	}
	printf("\n");	
	
}


void affiche_contrainte(){
     	int  i;
	i  =  1;		
	printf("        La contrainte :\n");
	printf("              %.2fX%d", coeff_contrainte[i],i);	
	for(i=2 ; i<= nb_variable ; i++){
	     printf(" + %.2fX%d", coeff_contrainte[i],i);	
	}	
	printf(" <= %.2f", coeff_contrainte[i]);
	printf("\n");	
}



void afficher_objet_trier(){
	int   i;
	printf("        Trier les objets :\n");
	printf("              ");
	for(i=1 ; i<= nb_variable ; i++){
	     printf("X%.0f    ",tab_indice[i]);	
	}
	printf("\n");
	
	
}

void affiche_result_node(arbre node){
	int  i;
    	printf("     S%d :",node->num_sommet);
	for(i=1 ; i<= nb_variable ; i++){
		printf("\n                X%d  =  %.2f  \n",i,node->valeurs_variables[i]);
	}
	printf("      E  =  %d",node->borne_superieur);
	printf("\n      La borne inferieur  =  %d \n",borne_inferieur);
}


void affiche_result_solution(){
	int   i;
	printf("        La solution : : \n");
	for(i=1 ;i<= nb_variable ; i++){
	    printf("              X%d  =  %d   \n",i, resultat_final[i]);
	}
	printf("              Z = %d  \n ",borne_inferieur);
	printf("\n");
}



void affich_arbre(arbre a){
	if(!est_vide(a)){
		printf("S%d  ::   %d \n",a->num_sommet,a->borne_superieur);
		printf("\n");
		affich_arbre(a->gauche);
		affich_arbre(a->droite);
	}
}


void  affiche_result(){
   printf("\n\n=======================================================================");
   printf("\n\n======================================================================\n\n");
              if(voir_detaille){
              	system("pause");
	}
	printf("  Le resultat :   \n");
	
	// afficher la fonction objective
  	affiche_fonction_objective();
		
	// afficher la contarinte
	affiche_contrainte();
	
	// affiche les objet par ordre d efficacite
	afficher_objet_trier();
	// affiche le résultat pour la premiere solution réalisable
	affiche_result_solution();
	
	// affiche l arbre
	   // affich_arbre(arbre_resultat);

	

}

    // recomancer ou quiter
int recomancer(){
        char chose[N];
        do{
            printf("\n    Voulez vous recomancer ou quiter :");
            printf("\n              1 : recomencer \n");
            printf("\n              2 : quiter \n");
           
            scanf("%s",&chose);
            choix = atoi(chose);	
        }while(choix != 1  && choix != 2);
        
        if(choix == 1){
        	return 1;
        }else{
        	return 0;
        }	
	 
}
  // afficehr les detaille ou pas 
void detaille_non(){
       char   detaiile[N];
       do{
            printf("     --  Voulez vous voir les detaille de la solution ou pas :");
            printf("\n             1 : voir les detaille");
            printf("\n             2 : ne pas voir les detaille \n");
            scanf("%s",detaiile);
            voir_detaille  = atoi(detaiile);
       }while(voir_detaille != 1 && voir_detaille !=2 );
       if(voir_detaille == 2){
            voir_detaille = 0 ;	
       }
}


// la fonction main
int main(){
    system("color f0");  // changer couleur
    // lire le nombre des variables
        lire_nb_variable();

    // lire la fonction objective :
        lire_fonction_objective();

    // lire la contrainte
        lire_contrainte();
   	 
    // initialiser efficacité (calculer l efficacité)
        initialiser_efficacite();
    
    // initialiser tableu d'indice (l ordre des objet)
        initialiser_indices();
    
    // trier efficacité avec et trier les objets
        tri_efficacite();
    
    // trouver une solution réalisable ( la borne inférieur )
       calcul_sol_realisable();
    
    // initialiser l arbre et lui donner un emplacement mémoire
       arbre_resultat = init_arbre() ;
       arbre_resultat=(arbre)malloc(sizeof(arborescence));
     
    // afficeher les détaille ou pas :
       detaille_non(); // si on veux voir les détaille

    // initialiser le vecteur des variable fixé par(-1) pas fixé
       initialise_var_fixe();

        
    // calculer S0 et faire la séparation
       separation_evaluation(arbre_resultat);
     

    // afficher le résultat
       affiche_result();

    
    // recomencer ou quiter
    if(recomancer()){
         num_som = 0 ; // recomancer du sommet S0
         main();  // recomancer	
    }	
    

    	
    return 0;
    
}
