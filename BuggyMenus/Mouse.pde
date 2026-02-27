void mouse(){
  if(state == START){
    fill(255, 204);   
  }
  if(state == MENU){
     fill(90, 204); 
  }
  final int offset = 10;
  rect(mouseX, mouseY, offset, offset);
}
