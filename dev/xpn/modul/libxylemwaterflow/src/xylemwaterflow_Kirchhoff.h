//void solveRoss(double *delT, class tree *T);
double fluxiToj(int i, int j, class tree *T);
double dKPdS(int i, class tree *T);
double dKdS(int i, double eta, class tree *T);
double dqidSi(int i, int j, double eta, class tree *T);
double dqidSj(int i, int j, double eta, class tree *T);
double returnNAN();
double returnINF();
double returnIND();
void testflux(int i, int j, class tree *T);
double fluxiToj_hom(int i, int j, class tree *T);
double dqidSi_hom(int i, int j, class tree *T);
double dqidSj_hom(int i, int j, class tree *T);
double estimate_timestep(class tree *T);
