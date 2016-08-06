void eclair(POINT p1, POINT p2, int nb_subdiv)
{
  POINT inc;
  int i;

  inc.x = (p2.x - p1.x) / nb_subdiv;
  inc.y = (p2.y - p1.y) / nb_subdiv;
  inc.z = (p2.z - p1.z) / nb_subdiv;

  glBegin( GL_LINE_STRIP );
  glVertex3fv(p1);
  for (i=0; i < nb_subdiv; i++) {
    glVertex3f( p1.x + inc.x + ((rand()%10)/100 - 0.5),
                p1.y + inc.y + ((rand()%10)/100 - 0.5),
                p1.z + inc.z + ((rand()%10)/100 - 0.5) )
  }
  glVertex3fv(p2);
  glEnd();
}

/*
Ensuite, il ne reste plus qu'a régler la couleur et l'épaisseur des
lignes, et avec un petit effet de flou par dessus tout cela, ça rend
encore mieux.
*/
