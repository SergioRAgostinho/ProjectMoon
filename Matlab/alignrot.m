function T = alignrot(R,A)

tic 
R = R./(ones(3,1)*sqrt(sum(R.*R)));
A = A./(ones(3,1)*sqrt(sum(A.*A)));

U_new = zeros(3);
max_idx = zeros(3,1);

R = eye(3);
A = [cos(1),-sin(1),0; 
     sin(1),cos(1),0;
    0,0,1];

for m = 1:3

    inner_prod = nan(3,1);
    for n = 1:3
        
      if any(max_idx == n)
        continue
      end
      
      inner_prod(n)= abs(R(:,n)'* A(:,m));
    end
    
    [~,max_idx_aux] = max(inner_prod);
    max_idx(m) = max_idx_aux;
    
    U_new(:,m) = sign(A(:,m)'*R(:,max_idx(m))) * R(:,max_idx(m));

end

T = U_new*A';

toc

end

