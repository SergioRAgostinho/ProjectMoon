
function R_a_aplicar_a_U2=R_match_axis(U1,U2)
% R_a_aplicar_a_U2=R_match_axis(U1,U2)
%
% Input: U1 - eixos ortogonais de tres arestas do paralelepipedo 1
%        U2 - eixos ortogonais de tres arestas do paralelepipedo 2
%
% Output: R_a_aplicar_a_U2 - matrix de rotacao a aplicar ao paralepipedo 2 
%                            tal que as suas arestas fiquem paralelas as arestas
%                            fiquem paralelas e prependiculares em relacao ao 
%                            paralelepipedo 1


tic
% so para garantir U1 e U2 tem vectores unitarios, se garantires por fora podes tirar
for n=1:3
  U1(:,n)=U1(:,n)/norm(U1(:,n));
end

for n=1:3
  U2(:,n)=U2(:,n)/norm(U2(:,n));
end


U_new=zeros(3,3);

min_idx=0*ones(3,1);
for m=1:3
    ext_prod=nan*ones(3,1);
    for n=1:3
      if any(min_idx==n)
        continue
      end
      ext_prod(n)=norm(cross(U1(:,n),U2(:,m)));
    end
    [min_val,min_idx_aux]=min(ext_prod);
    
    min_idx(m)=min_idx_aux;
    if U2(:,m)'*U1(:,min_idx(m))>0
      U_new(:,(m))=U1(:,min_idx(m));
    else
      U_new(:,(m))=-U1(:,min_idx(m));
    end

end

R_a_aplicar_a_U2=U_new*U2';

toc





%
% isto aqui ? a mesma coisa mas sem estar dentro de um ciclo
%ext_prod=nan*ones(3,1);
%for n=1:3
%  ext_prod(n)=norm(cross(U1(:,n),U2(:,1)));
%end
%[min_val,min_idx]=min(ext_prod);
%if U2(:,1)'*U1(:,min_idx)>0
%  U_new(:,1)=U1(:,min_idx);
%else
%  U_new(:,1)=-U1(:,min_idx);
%end
%
%
%
%
%ext_prod=nan*ones(3,1);
%for n=1:3
%  if n==min_idx
%    continue
%  end
%  ext_prod(n)=norm(cross(U1(:,n),U2(:,2)));
%end
%[min_val,min_idx2]=min(ext_prod);
%
%if U2(:,2)'*U1(:,min_idx2)>0
%  U_new(:,2)=U1(:,min_idx2);
%else
%  U_new(:,2)=-U1(:,min_idx2);
%end
%
%
%
%ext_prod=nan*ones(3,1)
%for n=1:3
%  if n==min_idx && n==min_idx2
%    continue
%  end
%  ext_prod(n)=norm(cross(U1(:,n),U2(:,3)));
%end
%[min_val,min_idx2]=min(ext_prod);
%
%if U2(:,3)'*U1(:,min_idx2)>0
%  U_new(:,3)=U1(:,min_idx2);
%else
%  U_new(:,3)=-U1(:,min_idx2);
%end
%
%
%R_a_aplicar_a_U2=U_new*U2'
%
%
%
