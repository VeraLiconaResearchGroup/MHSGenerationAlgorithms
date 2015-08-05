{Modified by Andrew Gainer-Dewar (2015) to build under Free Pascal}

program maximal_model;

const
  maxv = 255; {Was 288, but this is too large for Free Pascal's set implementation}
type
  variable_range = 1..maxv;
  variable = set of variable_range;
  pvariable = ^variable;
  setlist = ^setnode;
  setnode = record
	      vari : pvariable;
	      next : setlist;
	    end;
  genlist = ^gennode;
  gennode = record
	      vari : pvariable;
	      next : genlist;
	      com_part:genlist;
	      sep_part:genlist;
	    end;
  genvars = ^memdep;
  memdep = record
	    levelgen : genlist;
	    upper_lim: Cardinal;
	    next: genvars;
	   end;
  tokenlist = ^tokennode;
  tokennode = record
		tokenset:variable;
		links : Cardinal;
		next :tokenlist;
	      end;
  transversal = ^trannode;
  trannode = record
	       node  : genlist;
	       token : tokenlist;
	       next  : transversal;
	     end;
  dsetlist = ^dsetnode;
  dsetnode = record {This is a doubly-linked list of variables. It is used}
	       vari  : pvariable; {to store "monogamic",}
	       next  : dsetlist; { the set of clauses which are hit by only }
	       previous:dsetlist; { one node of the current transversal. }
	     end;


  stack_pointer= ^thunk;
  thunk = record
	    tp: transversal;
	    trail : genlist;
    	    af,sf0,sf1:transversal;
    	    endaf,endsf0,endsf1,endtp:transversal;
    	    action,k: Cardinal;
    	    buff:variable;
    	    maxk,limit:Cardinal;
    	    piece:transversal;
	    added_node:dsetlist;
	    level_mono:dsetlist;
	    end_level_mono:dsetlist;
{ Now the typical parameters that are called by name follow }
    	    t:transversal;
   	    depthpoint:genvars;
   	    cl:setlist;
    	    depth:Cardinal;
	    maintoken:variable;
{And the pointers required to form the doubly-linked list }
    	    next: stack_pointer;
    	    previous: stack_pointer;
	    prvpwr :Cardinal;
   	  end; {definition of thunk }

var
 count:Cardinal;
 nohelp,checksim,exinput,exoutput:boolean;
 genoutput,suppressoutput,sortoutput:boolean;
 outputinstance:boolean;
 outputscenario:integer;
 inname,outname:string;
 sortmode:string;
 i,j,k,m,n,c,depth : Cardinal;
 tcount,fcount,num:Cardinal;
 instance,models,cl : setlist;
 monogamic : dsetlist;
 memstart,endmemstart,tempmemstart : setlist;
 inf,outf: TextFile;
 tv,diaf1,diaf2,omega : variable;
 tempv,initt,initvi,vvt,gvv:transversal;
 free_space :transversal;
 token_space :tokenlist;
 clause_space:dsetlist;
 totaltime:Cardinal;
 success:boolean;
 mainstruct,depthpoint : genvars;
 outstep,viewstep:Cardinal;


 sspp,sspp_new,topstack:stack_pointer;

procedure printhelp;
{Prints a help message if the -h switch is on }
begin {printhelp}
  writeln;							{20a}
  writeln('Switches for program ');				{20a}
  writeln;							{20a}
  writeln('-h            : this help');				{20a}
  writeln;							{20a}
  writeln('-i <filename> : user defined input filename  - default inf.');{20a}
  writeln('              : Every hyperedge should be in a different line. ');
  writeln('              : A "0" in i-th place denotes that node i is in the' );
  writeln('              : hyperedge, any other symbol denotes that it is not.');
  writeln('              : Lines beginning with "#" are considered comments');
  writeln('-o <filename> : user defined output filename - default outf');{20a}
  writeln;							{20a}
  writeln('-c            : check simplicity             - default no');{20d}
  writeln('-q            : suppress any output on the screen');
  writeln('-sp <step>    : outputs a message every <step> transversals') ;
  writeln('              : (only when in regular form). Default 10000');
  writeln;							{20a}
  writeln('-s1           : output in generalized form');{20a}
  writeln('-s2           : output in regular form - default');		{20a}
  writeln('-s3           : output in regular form with sorting. Beware with');
  writeln('              : this one. Transversals are first stored and then sorted');
  writeln('              : so memory requirements may be high. Without it every');
  writeln('              : transversal is output and then forgotten.');
  writeln('-s4           : just counts the transversals');		{20a}
end;{printhelp}

procedure read_switches;
begin {read_switches}
nohelp:=true;
outputscenario:=2;
exinput:=false;		{if true the input filename is taken externally}
exoutput:=false;	{if true the output filename is taken externally}
checksim:=false;	{if true checks simplicity}
genoutput:=false;	{if false regular output enabled}
suppressoutput:=false;	{if true no screen output is produced }
sortoutput:=false;	{if true output is sorted before hand, for genoutput=false only}
sortmode:='no';
viewstep:=10000; { This is the default step in counting the transversals. }

if paramcount <> 0 then
	begin
	count:=0;
	while count<>paramcount do
		begin
		count:=count+1;
		if paramstr(count)='-h' then nohelp:=false;
		{Output will be in }
		if paramstr(count)='-s1' then outputscenario:=1;{generalized form}
		if paramstr(count)='-s2' then outputscenario:=2;{regular form}
		if paramstr(count)='-s3' then outputscenario:=3;{sorted regular}
		if paramstr(count)='-s4' then outputscenario:=4;{just counting it}
		{Input name}
		if paramstr(count)='-i' then
			if paramstr(count+1)<>'' then
				begin
				count:=count+1;
				exinput:=true;
				inname:=paramstr(count);
				end;
		{Output name}
		if paramstr(count)='-o' then
			if paramstr(count+1)<>'' then
				begin
				count:=count+1;
				exoutput:=true;
				outname:=paramstr(count);
				end;
		if paramstr(count)='-sp' then
			if paramstr(count+1)<>'' then
				begin
				count:=count+1;
				val(paramstr(count),viewstep);
				end;
		if paramstr(count)='-c' then checksim:=true;
		if paramstr(count)='-p' then outputinstance:=true;
		if paramstr(count)='-q' then suppressoutput:=true;
		if paramstr(count)='-sort' then
			if paramstr(count+1)<>'' then
				begin
				count:=count+1;
				sortmode:=paramstr(count);
    if (sortmode<>'asc') and (sortmode<>'desc') and (sortmode<>'no') then
	  writeln('Invalid sort mode, ignored - enter "asc" or "desc"');
				end;
		end;				{20a}
	end;

case outputscenario of
1:genoutput:=true;
2:genoutput:=false;
3:
	begin
	genoutput:=false;
	sortoutput:=true;
	end;
end; {case}

if (not suppressoutput) then
 begin
   if exinput then writeln('Input filename         ',inname);
   if exoutput then writeln('Output filename        ',outname);
   if checksim then writeln('Input instance will be checked for simplicity');
   case outputscenario of
	1:writeln('Output in generalized form');
	2:writeln('Output in regular form -default');
	3:writeln('Output in regular form with sorting');
	4:writeln('Only the number of transversals will be computed');
   end; {case}
 end; {if}

end;{read_switches}

procedure printtran (trans:transversal);
var tempt:transversal;
    i:Cardinal;

    begin
    tempt:=trans;
    tcount:=1;
    while tempt <> nil do
      begin
      fcount:=0;
        write(outf,'gn:');
        for i:=1 to n do
          if (i in tempt^.node^.vari^) then
		begin
		write(outf,i:3,',');
		fcount:=fcount+1;
		end;
	tcount:=tcount * fcount;
      tempt:=tempt^.next;
      end;
      num:=num + tcount;
      writeln(outf);
end; {printtran}

procedure print_models(instance:setlist; n,m:Cardinal);
  var
    i,j : Cardinal;
    current: setlist;
  begin
    current:=instance;
    for i:=1 to m do
      begin
        for j:=1 to n do
          if (j in current^.vari^) then write(outf,'0') else write(outf,'*');
	writeln(outf);
        current:=current^.next;
      end;
  end;  { print_models  }

procedure read_instance(var instance: setlist; var n,c:Cardinal);
  var i,j,class,up,down : Cardinal;
      buff: {packed array[1..1] of} char;
      temp,uppoint : setlist;
      a : array[1..maxv] of setlist;
      cline: boolean;
  begin { read_instance }
    n:=0;
    c:=0;
    for i:=1 to maxv do a[i]:=nil;
    while (not eof(inf)) do
      begin
	class:=0;
        new(temp);
	new(temp^.vari); { create a new set for the clause}
        temp^.next:=nil; { initially every clause is a }
        temp^.vari^:=[]; { single set of the false variables }
	j:=0;
	while (not eoln(inf)) do
          begin
            read(inf, buff);
	    if (buff='#') then
		begin
		  while (not eoln(inf)) do read(inf,buff);
		  readln(inf);
		end else
	    begin
	    j:=j+1;
            if (buff='0') {or (buff=' 0')} then
		begin
		  temp^.vari^ := temp^.vari^ + [j];
	    	  class:=class+1;
		end;
	    end;
          end; { while }
	if (n=0) then n:=j else
	  if (n<>j) then writeln('Error. Two lines with different number of nodes');
	c:=c+1;
	readln(inf);
	if (sortmode<>'asc') and (sortmode<>'desc') then
	begin
          temp^.next:=instance;
	  instance:=temp;
	end else
	if (sortmode='desc') then
	begin{ here starts sorted input desc}
	  if a[class]<>nil then
	    begin
	      temp^.next:=a[class]^.next;
	      a[class]^.next:=temp;
	    end else
	    begin
	      down:=class;
	      while (a[down]=nil) and (down<>1) do down:=down-1;
	      temp^.next:=a[down]; { this is always the next node in the list }
	      up:=class;
	      while (a[up]=nil) and (up<>n) do up:=up+1;
	      if a[up]=nil then instance:=temp
		else
		begin
		  uppoint:=a[up];
		  while (uppoint^.next<>temp^.next) do uppoint:=uppoint^.next;
		  uppoint^.next:=temp;
		end;
	      a[class]:=temp; { temp is the first edge with "class" nodes }
	    end;
	end else { here ends sorted input desc}
	begin{ here starts sorted input asc}
	  if a[class]<>nil then
	    begin
	      temp^.next:=a[class]^.next;
	      a[class]^.next:=temp;
	    end else
	    begin
	      up:=class;
	      while (a[up]=nil) and (up<>n) do up:=up+1;
	      temp^.next:=a[up]; { this is always the next node in the list }
	      down:=class;
	      while (a[down]=nil) and (down<>1) do down:=down-1;
	      if a[down]=nil then instance:=temp
		else
		begin
		  uppoint:=a[down];
		  while (uppoint^.next<>temp^.next) do uppoint:=uppoint^.next;
		  uppoint^.next:=temp;
		end;
	      a[class]:=temp; { temp is the first edge with "class" nodes }
	    end;
	end; { here ends sorted input asc}
      end;{ for i:=1 to c }
  end ; { read_instance }

procedure check_simplicity(instance:setlist);
  var
    ip,jp : setlist;
    error:boolean;
    begin
      error:=false;
      ip:=instance;
      while ip<> nil do
        begin
        jp:=ip^.next;
        while jp<> nil do
          begin
             if jp^.vari^ = ip^.vari^ then
		 begin
		   writeln('Alg: duplicate clause found');
		   error:=true;
		 end  else
             if (ip^.vari^ >= jp^.vari^) or
                (jp^.vari^ >= ip^.vari^) then
                 begin
                   error:=true;
		   writeln('Alg: containment found');
                 end;
	     jp:=jp^.next;
	  end;
	ip:=ip^.next;
        end;
        if not error then
	   begin
	   if (not suppressoutput) then writeln('Alg: instance was ok');
	   end;
    end; { check_simplicity }

procedure lex_sort(var instance : setlist);
  var
  temp,previoustemp,zerolist,endzerolist : setlist;
  j: Cardinal;

  begin { lex_sort }
    for j:= n downto 1 do
    begin
      zerolist:=nil;
      previoustemp:=nil;
      temp:=instance;
      while temp <> nil do
	  if not (j in temp^.vari^) then
	    begin
	      if previoustemp <> nil then previoustemp^.next:=temp;
	      if (j in instance^.vari^) and (previoustemp=nil)
		then instance:=temp;
	      previoustemp:=temp;
	      temp:=temp^.next;
	    end else
	    begin
	      if zerolist = nil then
		begin
		  zerolist:=temp;
		  endzerolist:=temp;
		end else
                begin
		  endzerolist^.next:=temp;
		  endzerolist:=temp;
		end;
	      temp:=temp^.next;
	    end;
      if previoustemp <> nil then previoustemp^.next:=zerolist;
      if zerolist<>nil then endzerolist^.next:=nil;
    end; { for loop }
  end; { lex_sort }


procedure assign_new_tvalue(var ttoken: tokenlist; tvalue: variable);
  begin
    if token_space= nil then new(ttoken) else
	begin
	  ttoken:=token_space;
	  token_space:=token_space^.next;
	end;
    ttoken^.tokenset:=tvalue;
    ttoken^.links:=1;
  end; { assign_new_tvalue }

procedure eq_tokens(var new_token, old_token: tokenlist);
  begin
    new_token:=old_token;
    old_token^.links:=old_token^.links+1;
  end; {eq_tokens}



procedure mynew(var tp:transversal);
  begin
    {mastercount:=mastercount+1;}
    if free_space =nil then new(tp)
      else
        begin
          tp:=free_space;
          free_space:=free_space^.next;
        end;
  end; { mynew }


procedure dispose_transversal(var tp:transversal; var endtp:transversal);
var temptp:transversal;
begin
temptp:=tp;
while temptp <> nil do
begin
    if temptp^.token^.links>1 then temptp^.token^.links:=temptp^.token^.links-1
      else
      begin
	temptp^.token^.next:=token_space;
	token_space:=temptp^.token;
      end;
    temptp:=temptp^.next;
end;
  if endtp <> nil then
    begin
      endtp^.next:=free_space;
      free_space:=tp;
    end;
end; { dispose_transversal }

procedure expand_and_store(var tp:transversal; var models:setlist);
  var
    a:array[1..maxv, 0..maxv] of integer;
    sel:array[1..maxv] of integer;
    tempt:transversal;
    j,i,imax,carry,length:Cardinal;
    tem:setlist;
    finished:boolean; varset:variable;

  begin
        tempt:=tp;
        imax:=0;
        while tempt <> nil do
        begin
          imax:=imax+1;
          length:=0;
          for j:=1 to n do
            if (j in tempt^.node^.vari^) then
              begin
                length:=length+1;
                a[imax,length]:=j;
              end;
          a[imax,length+1]:=0;
          a[imax,0]:=length;
          tempt:=tempt^.next;
        end; { end creating the a array for current tempt }
        for i:=1 to imax do sel[i]:=1;
        finished:=false;
        while not finished do
          begin
            varset:=[];
            for i:=1 to imax do varset:=varset + [a[i, sel[i]]];
            carry:=1;
            for i:=1 to imax do
            begin
              sel[i]:=sel[i]+carry;
              if sel[i]>a[i,0] then
              begin
                sel[i]:=1;
                carry:=1;
              end else carry:=0;
            end;
            if carry=1 then finished:=true else finished:=false;
	    case outputscenario of
	    3:  begin { must store all transversals to sort them }
            	  new(tem);
		  new(tem^.vari);
		  tem^.vari^:=varset;
            	  tem^.next:=models;
            	  models:=tem;
		end;
	    2:  begin   { no sorting, so we output immediately }
	          for i:=1 to n do
		  if (i in varset) then write(outf,'0') else write(outf,'*');
		  writeln(outf);
	        end;
	   end; { case }
            outstep:=outstep+1;
	    if (outstep = viewstep) then
	     begin
	       m:=m+outstep;
	       if (not suppressoutput) then writeln('Found ',m,' transversals...');
	       outstep:=0;
	     end;
          end; { while not finished }
   end; { expand_and_store }

procedure add_next_clause {(t:transversal; depthpoint:setlist;
cl:setlist; depth: Cardinal)};

 label
     entry_point,
     return_point,
     exit_point;
  var
    tempt: transversal;
    tempaf,tempsf0,tempsf1:transversal;
    temptp,temppoint:transversal;
    tempi:dsetlist;

    temk,ii:Cardinal;
    found_tran,haschanged:boolean;
    hitonce,hittwice,finished:boolean;
    inclu, xena:boolean;

    local_action : Cardinal;
    buff1,newmaintoken: variable;
    maxko2:Cardinal;


tempclause:setlist;
tempset:variable;

{ The declaration of the variables in thunk follows }

	    tp: transversal;
	    trail:genlist;
    	    af,sf0,sf1:transversal;
    	    endaf,endsf0,endsf1,endtp:transversal;
    	    action,k: Cardinal;
    	    buff:variable;
    	    maxk,limit:Cardinal;
    	    piece:transversal;
	    added_node:dsetlist;
	    level_mono:dsetlist;
	    end_level_mono:dsetlist;
{ Now the typical parameters that are called by name follow }
    	    t:transversal;
	    maintoken:variable;
            depthpoint : genvars;
   	    cl:setlist;
    	    depth:Cardinal;
	    prvpwr:Cardinal;


procedure print_trans(t:transversal);
var temptp:transversal;
    ii:Cardinal;
  begin
    temptp:=t;
    while temptp<>nil do
      begin
	for ii:=1 to n do if (ii in temptp^.node^.vari^) then write(ii:3);
	  write(' II ');
	temptp:=temptp^.next;
      end;
writeln;
end;

procedure add_monogamic(var cl:setlist);
 begin
    if clause_space = nil then new(added_node) else
      begin
	added_node:=clause_space;
	clause_space:=clause_space^.next;
      end;
    added_node^.previous:=nil;
    added_node^.vari:=cl^.vari;
    added_node^.next:=monogamic;
    if monogamic<> nil then monogamic^.previous:=added_node;
    monogamic:=added_node;
  end; { add_monogamic }

procedure remove_monogamic(var rem_clause:dsetlist);
  var tt:dsetlist;
  begin
    tt:=rem_clause^.next;
    rem_clause^.previous^.next:=rem_clause^.next; {there is always a "previous"}
    if rem_clause^.next<>nil then
	rem_clause^.next^.previous:=rem_clause^.previous;
    rem_clause^.previous:=nil;
    if level_mono<>nil then level_mono^.previous:=rem_clause
	else end_level_mono:=rem_clause;
    rem_clause^.next:=level_mono;
    level_mono:=rem_clause;
    rem_clause:=tt;
  end;


function generate_next_transversal:boolean;  {mem}


  begin { generate_next_transversal }
    tp:=nil;
    endtp:=nil;
    maxko2:=(limit+1) shr 1;{this has just a 1 in the first position of limit }
    case action of
    1: if k>limit then found_tran:=false else
	begin
	  found_tran:=true;
	  if k=limit then
	    begin
	      local_action:=1; { only one af node hits cl }
	      add_monogamic(cl);
	    end
	    else local_action:=2; { there is also some sf node hitting cl }
	end;
    2: if k>limit then found_tran:=false else
	begin
	  found_tran:=true;
	  local_action:=2;
	end;
    5: if k>=limit then
	begin
	  local_action:=4; {requires appropriate node,checking now }
          found_tran:=false;
          while (not found_tran) and (trail <> nil) do
            begin
	      if (trail^.vari^ * cl^.vari^ <> [])
		and (trail^.vari^ * maintoken = [])
		then begin
     		       found_tran:=true;
		       mynew(tp); { new transversal starts with this node }
		       tp^.next:=nil;
		       tp^.node:=trail;
		       assign_new_tvalue(tp^.token, cl^.vari^);
		       add_monogamic(cl);
		       endtp:=tp; { this is also the end of the transversal }
                     end;
	      trail:=trail^.next;
	    end;
	end { if k>=limit }
	else
	if (k=((prvpwr shr 1) or maxko2)) or (k=(limit shr 1))
	  then{ no hit from af and only one sf hit }
	begin
	  found_tran:=true;
	  local_action:=3; { means only one sf part hits cl }
          add_monogamic(cl);
	  prvpwr:=k;
	end else
	begin
	  found_tran:=true;
	  local_action:=2;
	end; { action=5 }
   end; {case}

    if found_tran then
      begin
       {  first make a copy of the af list }
        tempaf:=af;
	newmaintoken:=[];
        while tempaf <> nil do
          begin
            mynew(temptp);
            temptp^.node:=tempaf^.node;
            temptp^.next:=tp;
            tp:=temptp;
	    if (local_action=4) then assign_new_tvalue(temptp^.token,omega) else
	    if (local_action=1) and (piece^.node^.vari = tp^.node^.vari) then
	      assign_new_tvalue(temptp^.token, tempaf^.token^.tokenset * cl^.vari^)
		else eq_tokens(temptp^.token, tempaf^.token);
	    if (local_action <> 2) and (local_action <> 4)
		then  newmaintoken:=newmaintoken + temptp^.token^.tokenset;
	    if endtp = nil then endtp:=tp;
            tempaf:=tempaf^.next;
          end;
     {   then a copy of sf0 or sf1 according to the binary representation of k}
       temk:=k;
       tempsf0:=sf0;
       tempsf1:=sf1;
       while tempsf0 <> nil do
         begin
           mynew(temptp);
           if ((temk and 1) = 0) and (k<limit) then
	      begin
              temptp^.node:=tempsf0^.node;
	      if (local_action=3) and (k=prvpwr) then
	        assign_new_tvalue(temptp^.token, tempsf0^.token^.tokenset * cl^.vari^)
		else eq_tokens(temptp^.token,tempsf0^.token);
	      if (local_action <> 2)
		then  newmaintoken:=newmaintoken + temptp^.token^.tokenset;
	      end else
	      begin
              temptp^.node:=tempsf1^.node;
	      if (local_action = 4) then assign_new_tvalue(temptp^.token, omega)
		else eq_tokens(temptp^.token,tempsf1^.token);
	      if (local_action <> 2) and (local_action <> 4)
		then newmaintoken:=newmaintoken + temptp^.token^.tokenset;
	      end;
           temptp^.next:=tp;
           tp:=temptp;
	   if endtp = nil then endtp:=tp;
           tempsf0:=tempsf0^.next;
           tempsf1:=tempsf1^.next;
           temk:=temk shr 1;
         end;
      end; { if found_tran }

    if local_action=2 then newmaintoken:=maintoken;

    if found_tran and (local_action=4) then {update all tokens }
    begin
      tempi:=monogamic^.next; { the first clause is cl, so skip it }
      while (tempi<>nil) do
       begin
         if (tempi^.vari^ * endtp^.node^.vari^ <> []) then
		remove_monogamic(tempi) else
	 begin
	   temptp:=tp;
	   while (temptp <> nil) do
	     begin
	       if (tempi^.vari^ * temptp^.node^.vari^ <> []) then
		temptp^.token^.tokenset:=temptp^.token^.tokenset * tempi^.vari^;{here we don't use the token procedures, since we know what we're doing}
	       temptp:=temptp^.next;
	     end; { while }
	   tempi:=tempi^.next;
	 end;
       end; {while tempi<>nil}
      temptp:=tp;
	 while (temptp <> nil) do
	   begin
	     newmaintoken:=newmaintoken + temptp^.token^.tokenset;
	     temptp:=temptp^.next;
	   end;
    end ; { if local_action=4 }
    generate_next_transversal:=found_tran;
  end; { generate_next_transversal }

procedure pop_stack;
  begin { pop_stack }
    af:=topstack^.af;
    sf0:=topstack^.sf0;
    sf1:=topstack^.sf1;
    endaf:=topstack^.endaf;
    endsf0:=topstack^.endsf0;
    endsf1:=topstack^.endsf1;
    trail:=topstack^.trail;
    action:=topstack^.action;
    k:=topstack^.k;
    buff:=topstack^.buff;
    maxk:=topstack^.maxk;
    limit:=topstack^.limit;
    prvpwr:=topstack^.prvpwr;
    piece:=topstack^.piece;
    depthpoint:=topstack^.depthpoint;
    cl:=topstack^.cl;
    depth:=topstack^.depth;
    maintoken:=topstack^.maintoken;
    added_node:=topstack^.added_node;
    level_mono:=topstack^.level_mono;
    end_level_mono:=topstack^.end_level_mono;
  end; {pop_stack }

procedure push_stack;
  begin { push_stack }
    topstack^.trail:=trail;
    topstack^.af:=af;
    topstack^.sf0:=sf0;
    topstack^.sf1:=sf1;
    topstack^.endaf:=endaf;
    topstack^.endsf0:=endsf0;
    topstack^.endsf1:=endsf1;
    topstack^.action:=action;
    topstack^.k:=k;
    topstack^.buff:=buff;
    topstack^.maxk:=maxk;
    topstack^.limit:=limit;
    topstack^.prvpwr:=prvpwr;
    topstack^.piece:=piece;
    topstack^.depthpoint:=depthpoint;
    topstack^.cl:=cl;
    topstack^.depth:=depth;
    topstack^.added_node:=added_node;
    topstack^.level_mono:=level_mono;
    topstack^.end_level_mono:=end_level_mono;
	   if topstack^.next=nil then
	      begin { have reached end of stack. New node needed }
		new(sspp_new);
		sspp_new^.previous:=topstack;
		sspp_new^.next:=nil;
		topstack^.next:=sspp_new;
		topstack:=sspp_new;
	      end else { not end of stack. Simply move to next node }
		topstack:=topstack^.next;
	        topstack^.t:=tp;
		topstack^.maintoken:=newmaintoken;
	        topstack^.depthpoint:=depthpoint;
	        topstack^.cl:=cl^.next;
		topstack^.depth:=depth;
  end; {push_stack }

  begin { add_next_clause }
    entry_point:
{ Pop the typical parameters }
    t:=topstack^.t;
    maintoken:=topstack^.maintoken;
    depthpoint:=topstack^.depthpoint;
    cl:=topstack^.cl;
    depth:=topstack^.depth;
    depth:=depth+1;
{if ((depth div 1000)*1000=depth) then writeln(depth);}
    k:=0;
    level_mono:=nil;
    added_node:=nil;;
    maxk:=0;
    buff:=cl^.vari^;
    if (depthpoint^.upper_lim<depth) then
	begin haschanged:=true;	depthpoint:=depthpoint^.next; end
	else haschanged:=false;
    trail:=depthpoint^.levelgen;
    action:=5;
    af:=nil;
    sf0:=nil;
    sf1:=nil;
    endaf:=nil;
    endsf0:=nil;
    endsf1:=nil;
    while t<> nil do
      begin
	buff1:=t^.node^.vari^;
	if buff>=buff1 then inclu:=true else inclu:=false;
	if buff * buff1 = [] then xena:=true else xena:=false;
        if inclu or xena then
          begin
	    if inclu then
	        begin
		  if (action=5) then
		    begin
		      action:=1;
		      piece:=t;
		    end else if action=1 then action:=2;
                  if (haschanged) then t^.node:=t^.node^.com_part;
		end else
                if (haschanged) then t^.node:=t^.node^.sep_part;
            tempt:=t^.next;
	    t^.next:=af;
            af:=t;
	    t:=tempt;
	    if endaf=nil then endaf:=af; { this marks the end of the af list }
          end else
{	if (diaf <> []) and (diaf <> tempt^.vari) then }
          begin
	  {  diaf:=tempt^.vari-buff;}
	    maxk:=maxk+1;
	    mynew(tempsf1);
	    tempsf1^.node:=t^.node^.sep_part;
	    eq_tokens(tempsf1^.token, t^.token);
	    t^.node:=t^.node^.com_part;
	    tempt:=t^.next;
	    t^.next:=sf0;
	    sf0:=t;
	    t:=tempt;
	    if endsf0=nil then
	      begin
		endsf0:=sf0;{this marks the end of the sf0 list}
		endsf1:=sf1; {this marks the end of the sf1 list }
	      end;
	    tempsf1^.next:=sf1;
	    sf1:=tempsf1;
	    {if endsf1=nil then endsf1:=sf1;}
	  end;
      end;  { while tempt<> nil }
     limit:=(1 shl maxk)-1; { this computes 2**maxk-1 }
     prvpwr:=limit shr 1;

    while generate_next_transversal do	 {mem}
     begin

       { generate the k-th transversal that is produced from t by adding cl }
       if cl^.next =nil then { cl is the last clause to be added }
       begin { so we add the returned transversals to models }
	 if genoutput then printtran(tp) {output in generalized form }
	    else
	    expand_and_store(tp,models); { expand and save the transversal}
	 dispose_transversal(tp,endtp);
       end else
         begin
           push_stack; { Instead of recursive call of add_next_clause }
           goto entry_point;
return_point: { Here is where we return after finishing add_next_clause }
         end; { if cl^.nextclause=nil }
           if added_node <> nil then
	     begin
	       if added_node^.previous<>nil then
		 added_node^.previous^.next:=added_node^.next
		 else monogamic:=added_node^.next;
	       if added_node^.next<>nil then
		 added_node^.next^.previous:=added_node^.previous;
	       added_node^.next:=clause_space;
	       clause_space:=added_node;
	       added_node:=nil;
	     end;
	   if level_mono <> nil then {this level produced former monogamic }
	     begin { clause stored in level_mono list, which we now add }
	       end_level_mono^.next:=monogamic;   {to monogamic }
	       if monogamic<>nil then monogamic^.previous:=end_level_mono;
	       monogamic:=level_mono;
	       level_mono:=nil;
	     end;
       k:=k+1;
     end; { while generate_next_transversal loop }
  { Now return to free_space, lists af, sf0 and sf1 }
     dispose_transversal(af,endaf);
     dispose_transversal(sf0,endsf0);
     dispose_transversal(sf1,endsf1);
{ Fix typical parameters. Pop stack and return }
  if topstack^.previous=nil
   then goto exit_point { we have returned from the first call, end of proc }
   else { if not then pop }
      begin
	topstack:=topstack^.previous;
        pop_stack;
	goto return_point;
      end;
exit_point:

  end; { add_next_clause }


  procedure  build_main_data_structure;
  var
    endmainstruct,tempmainstruct : genvars;
    tempvv,vv,gvv : genlist;
    issplit:boolean;
    diaf1,diaf2,tv:variable; {tv stores the set of known vars up to this point}
    i,ii:Cardinal;
  begin { build_main_data_structure}
    cl:=instance;
    new(tempvv);
    new(tempvv^.vari);
    tempvv^.vari^:=cl^.vari^;
    tempvv^.next:=nil;
    tempvv^.com_part:=nil;
    tempvv^.sep_part:=nil;
    vv:=tempvv;
    tv:=tempvv^.vari^;
    new(mainstruct);
    mainstruct^.levelgen:=tempvv;
    mainstruct^.upper_lim:=1;
    mainstruct^.next:=nil;
    endmainstruct:=mainstruct; { this points to the last element of
		mainstruct structure }
    depthpoint:=mainstruct;
    for i:=2 to c do
      begin
	cl:=cl^.next;
	issplit:=false;
	while tempvv<>nil do
	  begin
	    diaf1:=tempvv^.vari^ * cl^.vari^;
	    diaf2:=tempvv^.vari^-diaf1;
	    if (diaf1<>[]) and (diaf2<>[]) then issplit:=true; {a node is split}
	    tempvv:=tempvv^.next;
	  end; { while loop }
	if (cl^.vari^ - tv) <> [] then issplit:=true; {cl has new nodes}
	if issplit then
	begin
	tempvv:=vv;
	vv:=nil; { a new vv list will be created, so initialize it }
	while tempvv<>nil do
	  begin
	    diaf1:=tempvv^.vari^ * cl^.vari^;
	    diaf2:=tempvv^.vari^-diaf1;
	    if (diaf1<>[]) and (diaf2<>[]) then
	    begin { this gen variable is split by cl}
	      new(gvv);        {for the common part}
	      new(gvv^.vari);
	      gvv^.vari^:=diaf1;
	      gvv^.next:=vv;
	      vv:=gvv;
	      tempvv^.com_part:=gvv;
	      begin gvv^.com_part:=nil; gvv^.sep_part:=gvv; end;
	      new(gvv);        {for the different part}
	      new(gvv^.vari);
	      gvv^.vari^:=diaf2;
	      gvv^.next:=vv;
	      vv:=gvv;
	      tempvv^.sep_part:=gvv;
	      begin gvv^.com_part:=nil; gvv^.sep_part:=gvv; end;
	    end else
	    begin {no split, simply point to the set at previous level}
	      new(gvv);
 	      gvv^.vari:=tempvv^.vari;
	      if (diaf1<>[]) then {next clause totally contains tempvv^.vari}
		begin tempvv^.com_part:=gvv; tempvv^.sep_part:=nil; end
		else begin tempvv^.com_part:=nil; tempvv^.sep_part:=gvv; end;
	      gvv^.next:=vv;
	      vv:=gvv;
	      begin gvv^.com_part:=nil; gvv^.sep_part:=gvv; end;
	    end;
	    tempvv:=tempvv^.next;
	  end; { while loop }
	if (cl^.vari^ - tv) <> [] then { cl has exclusive variables }
	  begin
	    new(gvv);
	    new(gvv^.vari);
	    gvv^.vari^:=cl^.vari^-tv;
	    gvv^.next:=vv;
	    vv:=gvv;
	    begin gvv^.com_part:=nil; gvv^.sep_part:=gvv; end;
	  end; { since issplit=true, create new node in mainstruct }
	  endmainstruct^.upper_lim:=i-1;{final level of validity of this list}
	  new(tempmainstruct); { build new node for next level }
	  tempmainstruct^.levelgen:=vv;
	  tempmainstruct^.upper_lim:=i; {first level of validity of new node}
	  tempmainstruct^.next:=nil;
	  endmainstruct^.next:=tempmainstruct;
	  endmainstruct:=tempmainstruct;
	  tv:=tv+cl^.vari^;
        end { if issplit }
	  else { current list of gen vars remains valid, so nothing is added}
	endmainstruct^.upper_lim:=i; {just update the level of validity}
	tempvv:=vv; {be ready for next scan}
      end; { for loop }
    tempmainstruct:=mainstruct;
  end; { build_main_data_structure}

begin { main }
  read_switches;
  if (not nohelp) then printhelp else
  begin			{20a}
	depth:=1;
	if exinput then assign(inf,inname)	{Default input file name inf}
		else assign(inf,'inf');
	begin
	  if exoutput then assign(outf,outname)
	    else assign(outf,'outf');
	  rewrite(outf);	{20e}
	end;
	reset(inf);
	instance:=nil;
	models:=nil;
	read_instance(instance,n,c);
	if checksim then check_simplicity(instance);

	num:=0;	{19fe} {3 counters for procedure printtran}
	m:=0;
	outstep:=0;
	omega:=[];
	for i:=1 to n do omega:=omega + [i];
	build_main_data_structure;
	cl:=instance;
	new(free_space);
	new(initt);{ This is the first transversal consisting of a single node}
	initt^.node:=mainstruct^.levelgen;
	cl:=instance;
	initt^.next:=nil;
	new(initt^.token); { Initialize the token of the first transversal }
	token_space:=nil;
	initt^.token^.tokenset:=cl^.vari^;
	clause_space:=nil;
	new(monogamic);
	monogamic^.vari:=cl^.vari;
	monogamic^.next:=nil;
	monogamic^.previous:=nil;
	  begin
	    writeln(outf,'# Output graph produced from input in file ',inname);
	    writeln(outf,'# Input graph nodes: ',n);
	    writeln(outf,'# Input graph hyperedges: ',c);
	    writeln(outf,'# Output graph hyperedges (below):             ') { reserve space for the unknown m};
	  end;
	new(sspp);
	sspp^.next:=nil;
	sspp^.previous:=nil;
	topstack:=sspp;
	topstack^.t:=initt;
	topstack^.maintoken:=initt^.token^.tokenset;
	topstack^.depthpoint:=depthpoint;
	topstack^.cl:=instance^.next;
	topstack^.depth:=depth;
{ First call of add_next_clause }
	if c>1 then
	add_next_clause{(initt,depthpoint,instance^.nextclause,depth{,totaltime,success)}}
	else if genoutput then printtran(initt)
	  else  expand_and_store(initt,models);
	m:=m+outstep; { collect what's left from last call }
	if (genoutput) then m:=num;
	if (not suppressoutput) then writeln('Found ',m,' transversals');
	if (not genoutput) and sortoutput then
		begin
		lex_sort(models);
		print_models(models,n,m);
		end;
	{if (not sortoutput) then} { now we must write m in outf }
	  begin
	    {seek(outf,0);}
	    writeln(outf,'# Output graph produced from input in file ',inname);
	    writeln(outf,'# Input graph nodes: ',n);
	    writeln(outf,'# Input graph hyperedges: ',c);
	    write(outf,'# Output graph hyperedges (below): ',m);
	  end;
	close(outf);
  end;								{20a}
end. { main }
