vertex = [7.62876 0.851078 3.2225 ;
                  -7.62876 0.851078 3.2225 ;
                  -7.62876 -0.851078 3.2225 ;
                  7.62876 0.851078 3.2225 ;
                  -7.62876 -0.851078 3.2225; 
                  7.62876 -0.851078 3.2225 ;
                  -7.62876 0.851078 3.2225 ;
                  7.62876 0.851078 3.2225 ;
                  7.62876 0.851078 -3.2225 ;
                  -7.62876 0.851078 3.2225 ;
                  7.62876 0.851078 -3.2225 ;
                  -7.62876 0.851078 -3.2225 ;
                  7.62876 -0.851078 -3.2225 ;
                  -7.62876 -0.851078 -3.2225 ;
                  -7.62876 0.851078 -3.2225 ;
                  7.62876 -0.851078 -3.2225 ;
                  -7.62876 0.851078 -3.2225 ;
                  7.62876 0.851078 -3.2225 ;
                  7.62876 -0.851078 3.2225 ;
                  -7.62876 -0.851078 3.2225 ;
                  -7.62876 -0.851078 -3.2225 ;
                  7.62876 -0.851078 3.2225 ;
                  -7.62876 -0.851078 -3.2225; 
                  7.62876 -0.851078 -3.2225 ;
                  7.62876 0.851078 -3.2225 ;
                  7.62876 0.851078 3.2225 ;
                  7.62876 -0.851078 3.2225 ;
                  7.62876 -0.851078 3.2225 ;
                  7.62876 -0.851078 -3.2225 ;
                  7.62876 0.851078 -3.2225 ;
                  -7.62876 0.851078 3.2225 ;
                  -7.62876 -0.851078 -3.2225; 
                  -7.62876 -0.851078 3.2225 ;
                  -7.62876 -0.851078 -3.2225 ;
                  -7.62876 0.851078 3.2225 ;
                  -7.62876 0.851078 -3.2225  ];
normals =   [0 0 1 ;
                  0 0 1 ;
                  0 0 1 ;
                  0 0 1 ;
                  0 0 1 ;
                  0 0 1 ;
                  0 1 0 ;
                  0 1 0 ;
                  0 1 0 ;
                  0 1 0 ;
                  0 1 0 ;
                  0 1 0 ;
                  0 0 -1 ;
                  0 0 -1 ;
                  0 0 -1 ;
                  0 0 -1 ;
                  0 0 -1 ;
                  0 0 -1 ;
                  0 -1 0 ;
                  0 -1 0 ;
                  0 -1 0 ;
                  0 -1 0 ;
                  0 -1 0 ;
                  0 -1 0 ;
                  1 0 0 ;
                  1 0 0 ;
                  1 0 0 ;
                  1 0 0 ;
                  1 0 0 ;
                  1 0 0 ;
                  -1 0 0 ;
                  -1 0 0 ;
                  -1 0 0 ;
                  -1 0 0 ;
                  -1 0 0 ;
                  -1 0 0];
              
vertex = [0 0 0; 1 0 0; 0 1 0;
            2 0 0; 3 0 0; 2 1 0;
            1 0 0; 2 0 0; 2 1 0];
normals = repmat([0,0,1],9,1);
           
locatorMid = [0.393701 0 0 0 ;
        0 -1.72092e-008 0.393701 0 ;
        0 -0.393701 -1.72092e-008 0 ;
        0 -0.0860091 0 1];
    
locatorTop = [0.393701 0 0 0 ;
        0 -1.72092e-008 0.393701 0 ;
        0 -0.393701 -1.72092e-008 0 ;
        0 -0.0860091 0.290763 1];
    
lM = locatorMid(4,1:3)';
lT = locatorTop(4,1:3)';

nVerts = size(vertex,1);
nTris = nVerts/3;

%another cuboid with a coincident face
vertex2 = vertex + [vertex(1)*2*ones(nVerts,1) zeros(nVerts,2)];

%having all the points
%1 - figure out the how many different planes exist

%should old contiguous triangles with the same normal as the same surface
%should also compute the total area of the surface
surfaces = zeros(nTris,5);
surfId = NaN(nVerts,1);
tic
sIdx = 1;
for n = 1:3:size(vertex,1)
   
    n1 = normals(n,:);
    idx = all(ones(nTris,1) * n1 == surfaces(:,1:3),2);
    if isempty(surfaces(idx,:))
        surfaces(sIdx,1:3) = n1;
        surfaces(sIdx,4) = surfaces(sIdx,4) + 1;
        surfId(n:n+2) = sIdx;
        sIdx = sIdx + 1;
    else
        surfaces(idx,4) = surfaces(idx,4) + 1;
        surfId(n:n+2) = find(idx);
    end
end

surfaces = surfaces(1:sIdx-1,:);
toc

%find contiguous triangles
contSurf = zeros(nTris,4)
cIdx = 1
id = 0

vertex4 = [vertex, zeros(nVerts,1)]
tic
for n = 1:sIdx - 1
    id = id + 1;
   idx = (surfId == n)
   if sum(idx) > 3
       vts = [vertex(idx,:), zeros(sum(idx),1)];
       nms = normals(idx,:);
       nVts = size(vts,1);
       nVtsTris = nVts/3;
       tempSurf = zeros(nVtsTris,4);
       
       %set the initial tri as the first contiguous group
       tempSurf(1,1:3) = nms(1,1:3);
       tempSurf(1,4) = id;
       vts(1:3,4) = id;
       
       for m = 4:3:nVts
           vtsIdx = 1:m-1;
           exists = all(((ones(numel(vtsIdx),1) * vts(m,1:3)) == vts(vtsIdx,1:3)),2) | ...
                all(((ones(numel(vtsIdx),1) * vts(m + 1,1:3)) == vts(vtsIdx,1:3)),2) | ...
                all(((ones(numel(vtsIdx),1) * vts(m + 2,1:3)) == vts(vtsIdx,1:3)) ,2);
           
            if any(exists)
               groupIds = unique(vts(exists,4));
               nUnique = numel(groupIds);
               uIdx = any(vts(:,4)*ones(1,nUnique) == ones(nVts,1)*groupIds',2);
               vts(uIdx,4) = groupIds(1) ;
               vts(m:m+2,4) = groupIds(1) ;
            else
                id = id + 1;
                vts(m:m+2,4) = id;
            end
       end
       
       vertex4(idx,4) = vts(:,4);
   else
       contSurf(cIdx,:) = surfaces(n,:);
       id = id + 1;
   end
   
   
   
end
toc

vertex4
% surfaces
% surfId