
case 0:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
  if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
    output_assembled_opcode(opt_tmp, "d%d ", opt_tmp->hex);
    i = inz;
    return SUCCEEDED;
  }
  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
    break;
 }
break;

case 1:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
  if (opt_tmp->op[x] == 'x') {
    y = i;
    i = inz;
    z = input_number();
    inz = i;
    i = y;
    if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
      return FAILED;
    if (z == SUCCEEDED && (d > 255 || d < -128))
      break;

    for (x++; x < OP_SIZE_MAX; inz++, x++) {
      if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	if (z == SUCCEEDED)
	  output_assembled_opcode(opt_tmp, "d%d d%d ", opt_tmp->hex, d);
	else if (z == INPUT_NUMBER_ADDRESS_LABEL)
	  output_assembled_opcode(opt_tmp, "k%d d%d Q%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	else {
	  output_assembled_opcode(opt_tmp, "d%d c%d ", opt_tmp->hex, latest_stack);
	}

	i = inz;
	return SUCCEEDED;
      }
      if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	break;
    }
  }
  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
    break;
 }
break;

case 2:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
  if (opt_tmp->op[x] == '?') {
    y = i;
    i = inz;
    z = input_number();
    inz = i;
    i = y;
    if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
      return FAILED;
    if (z == SUCCEEDED && (d > 65535 || d < -32768)) {
      print_error("Out of 16-bit range.\n", ERROR_NUM);
      return FAILED;
    }

    for (x++; x < OP_SIZE_MAX; inz++, x++) {
      if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	if (z == SUCCEEDED)
	  output_assembled_opcode(opt_tmp, "d%d y%d ", opt_tmp->hex, d);
	else if (z == INPUT_NUMBER_ADDRESS_LABEL)
	  output_assembled_opcode(opt_tmp, "k%d d%d r%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	else
	  output_assembled_opcode(opt_tmp, "d%d C%d ", opt_tmp->hex, latest_stack);

	i = inz;
	return SUCCEEDED;
      }
      if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	break;
    }
  }
  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
    break;
 }
break;

case 3:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
  if (opt_tmp->op[x] == 'x') {
    y = i;
    i = inz;
    parse_floats = NO;
    z = input_number();
    parse_floats = YES;
    inz = i;
    i = y;
    if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
      return FAILED;
    if (z == SUCCEEDED && (d > 255 || d < -128))
      break;

    e = d;
    v = z;
    h = latest_stack;
    if (z == INPUT_NUMBER_ADDRESS_LABEL)
      strcpy(labelx, label);

    for (x++; x < OP_SIZE_MAX; inz++, x++) {
      if (opt_tmp->op[x] == '~') {
	y = i;
	i = inz;
	z = input_number();
	inz = i;
	i = y;
	if (z != SUCCEEDED)
	  break;
	if (d > 7 || d < 0)
	  break;

	for (x++; x < OP_SIZE_MAX; inz++, x++) {
	  if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	    output_assembled_opcode(opt_tmp, "d%d ", opt_tmp->hex | (d << 5));

	    if (v == SUCCEEDED)
	      output_assembled_opcode(opt_tmp, "d%d ", e);
	    else if (v == INPUT_NUMBER_ADDRESS_LABEL)
	      output_assembled_opcode(opt_tmp, "k%d Q%s ", active_file_info_last->line_current, labelx);
	    else
	      output_assembled_opcode(opt_tmp, "c%d ", h);

	    i = inz;
	    return SUCCEEDED;
	  }
	  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	    break;
	}
      }
      if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	break;
    }
  }
  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
    break;
 }
break;

case 4:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
  if (opt_tmp->op[x] == 'x') {
    y = i;
    i = inz;
    parse_floats = NO;
    z = input_number();
    parse_floats = YES;
    inz = i;
    i = y;
    if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
      return FAILED;
    if (z == SUCCEEDED && (d > 255 || d < -128))
      break;

    e = d;
    v = z;
    h = latest_stack;
    if (z == INPUT_NUMBER_ADDRESS_LABEL)
      strcpy(labelx, label);

    for (x++; x < OP_SIZE_MAX; inz++, x++) {
      if (opt_tmp->op[x] == '~') {
	y = i;
	i = inz;
	z = input_number();
	inz = i;
	i = y;
	if (z != SUCCEEDED)
	  break;
	if (d > 7 || d < 0)
	  break;

	for (x++; x < OP_SIZE_MAX; inz++, x++) {
	  if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	    output_assembled_opcode(opt_tmp, "d%d ", opt_tmp->hex | (((d << 1) + 1) << 4));

	    if (v == SUCCEEDED)
	      output_assembled_opcode(opt_tmp, "d%d ", e);
	    else if (v == INPUT_NUMBER_ADDRESS_LABEL)
	      output_assembled_opcode(opt_tmp, "k%d Q%s ", active_file_info_last->line_current, labelx);
	    else
	      output_assembled_opcode(opt_tmp, "c%d ", h);

	    i = inz;
	    return SUCCEEDED;
	  }
	  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	    break;
	}
      }
      if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	break;
    }
  }
  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
    break;
 }
break;

case 5:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
  if (opt_tmp->op[x] == '~') {
    y = i;
    i = inz;
    z = input_number();
    inz = i;
    i = y;
    if (z != SUCCEEDED)
      break;
    if (d > 0xF || d < 0)
      break;

    for (x++; x < OP_SIZE_MAX; inz++, x++) {
      if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	output_assembled_opcode(opt_tmp, "d%d ", opt_tmp->hex | (d << 4));
	i = inz;
	return SUCCEEDED;
      }
      if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	break;
    }
  }
  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
    break;
 }
break;

case 0xA:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
  if (opt_tmp->op[x] == 'x') {
    y = i;
    i = inz;
    z = input_number();
    inz = i;
    i = y;
    if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
      return FAILED;
    if (z == SUCCEEDED && (d > 255 || d < -128))
      break;

    e = d;
    v = z;
    h = latest_stack;
    if (z == INPUT_NUMBER_ADDRESS_LABEL)
      strcpy(labelx, label);

    for (x++; x < OP_SIZE_MAX; inz++, x++) {
      if (opt_tmp->op[x] == 'x') {
	y = i;
	i = inz;
	z = input_number();
	inz = i;
	i = y;
	if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	  return FAILED;
	if (z == SUCCEEDED && (d > 255 || d < -128))
	  break;

	for (x++; x < OP_SIZE_MAX; inz++, x++) {
	  if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	    if (v == SUCCEEDED)
	      output_assembled_opcode(opt_tmp, "d%d d%d ", opt_tmp->hex, e);
	    else if (v == INPUT_NUMBER_ADDRESS_LABEL)
	      output_assembled_opcode(opt_tmp, "k%d d%d Q%s ", active_file_info_last->line_current, opt_tmp->hex, labelx);
	    else
	      output_assembled_opcode(opt_tmp, "d%d c%d ", opt_tmp->hex, h);

	    if (z == SUCCEEDED)
	      output_assembled_opcode(opt_tmp, "d%d ", d);
	    else if (z == INPUT_NUMBER_ADDRESS_LABEL)
	      output_assembled_opcode(opt_tmp, "k%d R%s ", active_file_info_last->line_current, label);
	    else {
	      output_assembled_opcode(opt_tmp, "c%d ", latest_stack);
	      /* let's configure the stack so that all label references inside are relative */
	      stacks_tmp->relative_references = 1;
	    }

	    i = inz;
	    return SUCCEEDED;
	  }
	  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	    break;
	}
      }
      if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	break;
    }
  }
  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
    break;
 }
break;

case 0xB:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
  if (opt_tmp->op[x] == 'x') {
    y = i;
    i = inz;
    parse_floats = NO;
    z = input_number();
    parse_floats = YES;
    inz = i;
    i = y;
    if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
      return FAILED;
    if (z == SUCCEEDED && (d > 255 || d < -128))
      break;

    e = d;
    v = z;
    h = latest_stack;
    if (z == INPUT_NUMBER_ADDRESS_LABEL)
      strcpy(labelx, label);

    for (x++; x < OP_SIZE_MAX; inz++, x++) {
      if (opt_tmp->op[x] == 'x') {
	y = i;
	i = inz;
	z = input_number();
	inz = i;
	i = y;
	if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	  return FAILED;
	if (z == SUCCEEDED && (d > 255 || d < -128))
	  break;

	for (x++; x < OP_SIZE_MAX; inz++, x++) {
	  if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	    if (z == SUCCEEDED)
	      output_assembled_opcode(opt_tmp, "d%d d%d ", opt_tmp->hex, d);
	    else if (z == INPUT_NUMBER_ADDRESS_LABEL)
	      output_assembled_opcode(opt_tmp, "k%d d%d Q%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	    else
	      output_assembled_opcode(opt_tmp, "d%d c%d ", opt_tmp->hex, latest_stack);

	    if (v == SUCCEEDED)
	      output_assembled_opcode(opt_tmp, "d%d ", e);
	    else if (v == INPUT_NUMBER_ADDRESS_LABEL)
	      output_assembled_opcode(opt_tmp, "k%d Q%s ", active_file_info_last->line_current, labelx);
	    else
	      output_assembled_opcode(opt_tmp, "c%d ", h);

	    i = inz;
	    return SUCCEEDED;
	  }
	  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	    break;
	}
      }
      if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	break;
    }
  }
  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
    break;
 }
break;

case 0xC:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
  if (opt_tmp->op[x] == 'x') {
    y = i;
    i = inz;
    parse_floats = NO;
    z = input_number();
    parse_floats = YES;
    inz = i;
    i = y;
    if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
      return FAILED;
    if (z == SUCCEEDED && (d > 255 || d < -128))
      break;

    e = d;
    v = z;
    h = latest_stack;
    if (z == INPUT_NUMBER_ADDRESS_LABEL)
      strcpy(labelx, label);

    for (x++; x < OP_SIZE_MAX; inz++, x++) {
      if (opt_tmp->op[x] == '~') {
	y = i;
	i = inz;
	z = input_number();
	inz = i;
	i = y;
	if (z != SUCCEEDED)
	  break;
	if (d > 7 || d < 0)
	  break;
	g = d;

	for (x++; x < OP_SIZE_MAX; inz++, x++) {
	  if (opt_tmp->op[x] == 'x') {
	    y = i;
	    i = inz;
	    z = input_number();
	    inz = i;
	    i = y;
	    if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	      return FAILED;
	    if (z == SUCCEEDED && (d > 255 || d < -128))
	      break;

	    for (x++; x < OP_SIZE_MAX; inz++, x++) {
	      if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
		output_assembled_opcode(opt_tmp, "d%d ", opt_tmp->hex | (g << 5));

		if (v == SUCCEEDED)
		  output_assembled_opcode(opt_tmp, "d%d ", e);
		else if (v == INPUT_NUMBER_ADDRESS_LABEL)
		  output_assembled_opcode(opt_tmp, "k%d Q%s ", active_file_info_last->line_current, labelx);
		else
		  output_assembled_opcode(opt_tmp, "c%d ", h);

		if (z == SUCCEEDED)
		  output_assembled_opcode(opt_tmp, "d%d ", d);
		else if (z == INPUT_NUMBER_ADDRESS_LABEL)
		  output_assembled_opcode(opt_tmp, "k%d R%s ", active_file_info_last->line_current, label);
		else {
		  output_assembled_opcode(opt_tmp, "c%d ", latest_stack);
		  /* let's configure the stack so that all label references inside are relative */
		  stacks_tmp->relative_references = 1;
		}

		i = inz;
		return SUCCEEDED;
	      }
	      if (opt_tmp->op[x] != toupper((int)buffer[inz]))
		break;
	    }
	  }
	  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	    break;
	}
      }
      if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	break;
    }
  }
  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
    break;
 }
break;

case 0xD:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
  if (opt_tmp->op[x] == 'x') {
    y = i;
    i = inz;
    parse_floats = NO;
    z = input_number();
    parse_floats = YES;
    inz = i;
    i = y;
    if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
      return FAILED;
    if (z == SUCCEEDED && (d > 255 || d < -128))
      break;

    e = d;
    v = z;
    h = latest_stack;
    if (z == INPUT_NUMBER_ADDRESS_LABEL)
      strcpy(labelx, label);

    for (x++; x < OP_SIZE_MAX; inz++, x++) {
      if (opt_tmp->op[x] == '~') {
	y = i;
	i = inz;
	z = input_number();
	inz = i;
	i = y;
	if (z != SUCCEEDED)
	  break;
	if (d > 7 || d < 0)
	  break;
	g = d;

	for (x++; x < OP_SIZE_MAX; inz++, x++) {
	  if (opt_tmp->op[x] == 'x') {
	    y = i;
	    i = inz;
	    z = input_number();
	    inz = i;
	    i = y;
	    if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	      return FAILED;
	    if (z == SUCCEEDED && (d > 255 || d < -128))
	      break;

	    for (x++; x < OP_SIZE_MAX; inz++, x++) {
	      if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
		output_assembled_opcode(opt_tmp, "d%d ", opt_tmp->hex | (((g << 1) + 1) << 4));

		if (v == SUCCEEDED)
		  output_assembled_opcode(opt_tmp, "d%d ", e);
		else if (v == INPUT_NUMBER_ADDRESS_LABEL)
		  output_assembled_opcode(opt_tmp, "k%d Q%s ", active_file_info_last->line_current, labelx);
		else
		  output_assembled_opcode(opt_tmp, "c%d ", h);

		if (z == SUCCEEDED)
		  output_assembled_opcode(opt_tmp, "d%d ", d);
		else if (z == INPUT_NUMBER_ADDRESS_LABEL)
		  output_assembled_opcode(opt_tmp, "k%d R%s ", active_file_info_last->line_current, label);
		else {
		  output_assembled_opcode(opt_tmp, "c%d ", latest_stack);
		  /* let's configure the stack so that all label references inside are relative */
		  stacks_tmp->relative_references = 1;
		}

		i = inz;
		return SUCCEEDED;
	      }
	      if (opt_tmp->op[x] != toupper((int)buffer[inz]))
		break;
	    }
	  }
	  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	    break;
	}
      }
      if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	break;
    }
  }
  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
    break;
 }
break;

case 0xE:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
  if (opt_tmp->op[x] == 'x') {
    y = i;
    i = inz;
    z = input_number();
    inz = i;
    i = y;
    if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
      return FAILED;
    if (z == SUCCEEDED && (d > 255 || d < -128))
      break;

    for (x++; x < OP_SIZE_MAX; inz++, x++) {
      if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	if (z == SUCCEEDED)
	  output_assembled_opcode(opt_tmp, "d%d d%d ", opt_tmp->hex, d);
	else if (z == INPUT_NUMBER_ADDRESS_LABEL)
	  output_assembled_opcode(opt_tmp, "k%d d%d R%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	else {
	  output_assembled_opcode(opt_tmp, "d%d c%d ", opt_tmp->hex, latest_stack);
      stacks_tmp->relative_references = 1;
	}

	i = inz;
	return SUCCEEDED;
      }
      if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	break;
    }
  }
  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
    break;
 }
break;

case 0xF:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
  if (opt_tmp->op[x] == '?') {
    y = i;
    i = inz;
    parse_floats = NO;
    z = input_number();
    parse_floats = YES;
    inz = i;
    i = y;
    if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
      return FAILED;
    if (z == SUCCEEDED && (d > 8191 || d < 0))
      break;

    e = d;
    v = z;
    h = latest_stack;
    if (z == INPUT_NUMBER_ADDRESS_LABEL)
      strcpy(labelx, label);

    for (x++; x < OP_SIZE_MAX; inz++, x++) {
      if (opt_tmp->op[x] == '~') {
	y = i;
	i = inz;
	z = input_number();
	inz = i;
	i = y;
	if (z != SUCCEEDED)
	  break;
	if (d > 7 || d < 0)
	  break;

	for (x++; x < OP_SIZE_MAX; inz++, x++) {
	  if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	    if (v == SUCCEEDED)
	      output_assembled_opcode(opt_tmp, "d%d y%d ", opt_tmp->hex, e | d << 13);
	    else if (v == INPUT_NUMBER_ADDRESS_LABEL)
	      output_assembled_opcode(opt_tmp, "k%d d%d n%d %s ", active_file_info_last->line_current, opt_tmp->hex, d, labelx);
	    else
	      output_assembled_opcode(opt_tmp, "d%d N%d %d ", opt_tmp->hex, d, h);

	    i = inz;
	    return SUCCEEDED;
	  }
	  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	    break;
	}
       }
      if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	break;
    }
  }
  if (opt_tmp->op[x] != toupper((int)buffer[inz]))
    break;
 }
break;

