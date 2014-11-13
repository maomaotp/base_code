function selectAll(obj) 
{ 
for(var i = 0;i<obj.elements.length;i++) 
if(obj.elements[i].type == "checkbox") 
obj.elements[i].checked = true; 
} 
function selectOther(obj) 
{ 
for(var i = 0;i<obj.elements.length;i++) 
if(obj.elements[i].type == "checkbox" ) 
{ 
if(!obj.elements[i].checked) 
obj.elements[i].checked = true; 
else 
obj.elements[i].checked = false; 
} 
} 
